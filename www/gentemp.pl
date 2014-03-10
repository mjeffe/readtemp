#!/usr/bin/perl
# generate some "temperature" readings like readtemp would produce - for web page development

use strict;
use warnings;

$| = 1;

my $t0 = 25;  # base temperature
my $h0 = 35;  # base humidity
my $t1 = 25;  # base temperature
my $h1 = 35;  # base humidity

while (1) {

   # randomly, add or subtract 0 or 1 to temp
   if ( int(rand(100)) > 50 ) {
      $t0 += int(rand(2)); 
   } else {
      $t0 -= int(rand(2)); 
   }
   # randomly, add or subtract 0 or 1 to temp
   if ( int(rand(100)) > 50 ) {
      $t1 += int(rand(2)); 
   } else {
      $t1 -= int(rand(2)); 
   }


   # randomly, add or subtract 0 or 1 to humidity
   if ( int(rand(100)) > 50 ) {
      $h0 += int(rand(2)); 
   } else {
      $h0 -= int(rand(2)); 
   }
   if ( int(rand(100)) > 50 ) {
      $h1 += int(rand(2)); 
   } else {
      $h1 -= int(rand(2)); 
   }

   my ($sec,$min,$hour,$day,$mon,$year,$wday,$yday,$isdst) = localtime;
   $year += 1900;
   printf("0|%d-%02d-%02d %02d:%02d:%02d|%s|%s\n", $year,$mon,$day,$hour,$min,$sec,$t0,$h0);
   printf("1|%d-%02d-%02d %02d:%02d:%02d|%s|%s\n", $year,$mon,$day,$hour,$min,$sec,$t1,$h1);

   sleep(2);
}

