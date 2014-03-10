#!/usr/bin/perl

use strict;
use warnings;

my $RT = '/home/pi/src/readtemp/readtemp';

# sensors, and which gpio pin they are connected to
my @temp_sensor_pins = (4,17);

while (1) {

   for ( my $i=0; $i < scalar @temp_sensor_pins; $i++ ) {
      my $data = '';
      do {
         $data = `$RT $temp_sensor_pins[$i]`;
         chomp($data);
         if ( $data ) {
            #print scalar localtime . '|' . join('|', split(/\s/, $data)) . "\n";
            my ($sec,$min,$hour,$day,$mon,$year,$wday,$yday,$isdst) = localtime;
            $year += 1900;
            printf("%d|%d-%02d-%02d %02d:%02d:%02d|%s\n", 
               $i,$year,$mon,$day,$hour,$min,$sec,join('|', split(/\s/, $data)));
         } else {
            #print "sleeping 3\n";
            sleep(3);
         }
      } while ( ! $data );
   }

   #print "sleeping 30\n";
   sleep(30);  
}


