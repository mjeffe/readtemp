/* **************************************************************************
 * $Id$
 *
 * This project is free software. You can redistribute it and/or modify it
 * under the terms of the FreeBSD License which follows.
 * --------------------------------------------------------------------------
 * Copyright (c) 2011, Matt Jeffery (mjeffe@gmail.com). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------

 * Read temperature and humidity values from DHT11 sensor
 *
 * Based on code from Adafruit:
 *
 *    git clone git://github.com/adafruit/Adafruit-Raspberry-Pi-Python-Code.git
 *    cd Adafruit_DHT_Driver
 *
 * This depends on the bcm2835 library for reading GPIO pins on RPi.
 *
 *    http://www.airspayce.com/mikem/bcm2835/index.html
 *
 * Some helpful information about how the DHT11/22 works can be found here (some
 * of which I've included in comments throughout the code):
 *
 *    http://raspberrypi.stackexchange.com/questions/7897/help-to-understand-the-dht22-c-code
 *
 * *************************************************************************/


// Access from ARM Running Linux
#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <bcm2835.h>
#include <unistd.h>

#define MAXTIMINGS 100
//#define DEBUG

/* globals */
int verbose = 0;

/* prototypes */
int readDHT(int pin);


/* --------------------------------------------------------------------------
 * Main
 * ------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
   int dhtpin = 0;

   if (argc < 2) {
      printf("Description:\n");
      printf("   Read from a DHT11 temperature/humidity sensor connected to GPIO pin.\n");
      printf("   Output is: degC %hum\n");
      printf("Usage: %s [-v] gpio_pin\n", argv[0]);
      printf("Where: \n");
      printf("   -v        produces more verbose output\n");
      printf("   gpio_pin  the Pi's GPIO pin the sensor is connected to\n");
      printf("\n");
      return 2;
   }

   if (!bcm2835_init())
      return 1;

   if ( argc == 3  && strcmp(argv[1], "-v") == 0 ) {
      verbose = 1;
      dhtpin = atoi(argv[2]);
   } else {
      dhtpin = atoi(argv[1]);
   }

   if (dhtpin <= 0) {
      printf("Please select a valid GPIO pin #\n");
      return 3;
   }

   if ( verbose )
      printf("Using pin #%d\n", dhtpin);
   readDHT(dhtpin);
   return 0;

}


/* --------------------------------------------------------------------------
 * Read temp from DHT11 temperature/humidity sensor
 * ------------------------------------------------------------------------*/
int readDHT(int pin) {
   int bits[250], data[100];
   int bitidx = 0;

   int counter = 0;
   int laststate = HIGH;
   int j=0;

   /* send DHT11 the start signal.
    * This seems to be the opposite sequence from what the datasheet says... */
   bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP); // Set GPIO pin to output
   bcm2835_gpio_write(pin, HIGH);
   usleep(500000);  // 500 ms
   bcm2835_gpio_write(pin, LOW);
   usleep(20000);

   /* start listening for DHT11's response */

   bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT); // Set GPIO pin to input
   data[0] = data[1] = data[2] = data[3] = data[4] = 0;

   /* wait for pin to drop. This signals begin of sensor's sending data */
   while (bcm2835_gpio_lev(pin) == 1) {
      usleep(1);
   }

   /*
    * The sensor will be transmitting it's data by changing the value from low
    * to high to low and so on. If the bit to be transmitted is 1, the high
    * state of the pin will last 70 uSeconds, if it's 0, it will last 26-28 uSec.
    *
    * Here we loop, watching for the pin level to change. Then we count the
    * number of loops that the signal level did not change. Based on that we
    * can estimate the length of the signal in uSeconds. If the counter value
    * get's to 1000, however, the loop ** will brake and data reading will be
    * finished. This should happen before MAXTIMINGS bits where transmitted 
    * but this value is a safeguard if something went wrong. 
    */
   // read data!
   for (int i=0; i< MAXTIMINGS; i++) {
      counter = 0;
      while ( bcm2835_gpio_lev(pin) == laststate) {
         counter++;
         //nanosleep(1);      // overclocking might change this?
         /* seems like we should have a usleep(1) here. Arduino code does... */
         if (counter == 1000)
            break;
      }
      laststate = bcm2835_gpio_lev(pin);
      if (counter == 1000) break;
      bits[bitidx++] = counter;

      /*
       * I don't understand why counter is compared to 200.
       * The counter is updated ~ every uSec? According to the data sheet,
       * a logical 1 is when the pin is held in a hight state for 70 uSec
       * so it seems like we should be comparing it to 70 uSec. Perhaps
       * the 200 is accounting for the CPU going off and doing other things?
       *
       * note: j/8 points to the same data[] bucket for 8 consecutive 
       * values of j. That is 1 byte's worth of data.
       *
       * Also, we ignore the first three transitions. Those are the LOW,
       * HIGH, LOW signalling the beggining of sensor's data transmision.
       * I think the i%2 says, we only care about every other transition,
       * that is, we only want to push the data bit on the HIGH to LOW
       * transition, not the LOW to HIGH.
       */
      if ((i>3) && (i%2 == 0)) {
         // shove each bit into the storage bytes
         data[j/8] <<= 1;
         if (counter > 200)
            data[j/8] |= 1;
         j++;
      }
   }


#ifdef DEBUG
   for (int i=3; i<bitidx; i+=2) {
      printf("bit %d: %d\n", i-3, bits[i]);
      printf("bit %d: %d (%d)\n", i-2, bits[i+1], bits[i+1] > 200);
   }
#endif

   if ( verbose )
      printf("Data (%d): 0x%x 0x%x 0x%x 0x%x 0x%x\n", j, data[0], data[1], data[2], data[3], data[4]);

   /*
    * Arduino code has the following comment here. Also has (j >= 40) :
    *
    * // check we read 40 bits and that the checksum matches
    *
    */
   if ((j >= 39) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
      // yay!
      if ( verbose )
         printf("Temp = %d *C, Hum = %d \%\n", data[2], data[0]);
      else
         printf("%d %d\n", data[2], data[0]);
      return 1;
   }

   return 0;
}
