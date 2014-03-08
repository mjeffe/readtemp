#!/usr/bin/perl

use strict;
use warnings;

my $RT = '/home/pi/src/readtemp/readtemp 4';

while (1) {
   my $data = '';
   do {
      $data = `$RT`;
      chomp($data);
      if ( $data ) {
         #print scalar localtime . '|' . join('|', split(/\s/, $data)) . "\n";
         my ($sec,$min,$hour,$day,$mon,$year,$wday,$yday,$isdst) = localtime;
         $year += 1900;
         printf("%d-%02d-%02d %02d:%02d:%02d|%s\n", 
            $year,$mon,$day,$hour,$min,$sec,join('|', split(/\s/, $data)));
      } else {
         #print "sleeping 3\n";
         sleep(3);
      }
   } while ( ! $data );

   $data = '';
   #print "sleeping 27\n";
   sleep(27);  # 27 + 3 = 30 second loop
}


