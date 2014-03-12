/* **************************************************************************
 * $Id$
 *
 * Read temperature and humidity values from DHT11 sensor
 *
 * Based on code from Adafruit:
 *
 *    git clone git://github.com/adafruit/Adafruit-Raspberry-Pi-Python-Code.git
 *    cd Adafruit_DHT_Driver
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

   /* send DHT11 the start signal */
   bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP); // Set GPIO pin to output
   bcm2835_gpio_write(pin, HIGH);
   usleep(500000);  // 500 ms
   bcm2835_gpio_write(pin, LOW);
   usleep(20000);

   /* start listening for DHT11's response */

   bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT); // Set GPIO pin to input
   data[0] = data[1] = data[2] = data[3] = data[4] = 0;

   // wait for pin to drop? not sure what or why...
   while (bcm2835_gpio_lev(pin) == 1) {
      usleep(1);
   }

   /*
    * The sensor will be transmitting it's data by changing the value from low
    * to high to low and so on. If the bit to be transmitted is 1, the high
    * state of the pin will be longer, if it's 0, it will be shorter.
    *
    * So what your code is doing is to probe the value in a loop and watch if
    * the signal level changes, counting number of loops that the signal did
    * not change. This way you know for how long it was in the last known state
    * (counter variable). This counter is then used to check if the bit that
    * was send is 1 or 0. If the counter value get's to 1000, however, the loop
    * will brake and data reading will be finished.
    *
    * This should happen before MAXTIMINGS bits where transmitted but this
    * value is a safeguard if something went wrong. 
    */
   // read data!
   for (int i=0; i< MAXTIMINGS; i++) {
      counter = 0;
      while ( bcm2835_gpio_lev(pin) == laststate) {
         counter++;
         //nanosleep(1);      // overclocking might change this?
         if (counter == 1000)
            break;
      }
      laststate = bcm2835_gpio_lev(pin);
      if (counter == 1000) break;
      bits[bitidx++] = counter;

      if ((i>3) && (i%2 == 0)) {
         // shove each bit into the storage bytes
         // note: j/8 points to the same data[] bucket for 8 consecutive 
         // values of j. That is 1 byte's worth of data.
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
