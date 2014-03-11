#!/usr/bin/perl
# ---------------------------------------------------------------------------
# $Id$
#
# daemon for tracking temp and humidity
#
# calls the readtemp binary at regular intervals and outputs the specific
# format and log file that the web front end expects.
# ---------------------------------------------------------------------------

use strict;
use warnings;

#use Proc::Daemon;
#use Proc::PID::File;
my $keep_running = 1;
#$SIG{TERM} = sub { $keep_running = 0 };

#
# config globals
#

# readtemp binary
my $RT = '/usr/local/bin/readtemp';

# sensors, and which gpio pin they are connected to
my @temp_sensor_pins = (4,17);

# directory for log file
my $logdir = '/var/www/pi';

# how often we should take a reading - in seconds
my $read_interval = 30;


#
# prototypes
#
sub read_sensors($);
sub get_log_name();




# ---------------------------------------------------------------------------
# -------------                MAIN                       -------------------
# ---------------------------------------------------------------------------
sub main() {
   # set up stuff to run as daemon
   # note: should write pidfile, /var/log log, etc, but I'm lazy...
   # see http://stackoverflow.com/questions/766397/how-can-i-run-a-perl-script-as-a-system-daemon-in-linux
   # see: http://www.perlmonks.org/?node_id=478839
   #Proc::Daemon::Init();
   # exit if already running
   #if ( Proc::PID::File->running() ) {
   #   exit(0);
   #}

   my $current_log = get_log_name();
   my $LOG = undef;
   open($LOG, '>>', $current_log) || die "Unable to open log file $current_log\n";
   select((select($LOG), $| = 1)[0]); # turn on autoflushing for this file handle

   while ($keep_running) {  # play like I'm a daemon...

      # Roll over (rotate) log whenever get_log_name() sends us a new file name
      my $log = get_log_name();
      if ( $current_log ne $log ) {
         close($LOG) || die "Unable to close file $current_log\n";
         $current_log = $log;
         open($LOG, '>>', $current_log) || die "Unable to open log file $current_log\n";
         select((select($LOG), $| = 1)[0]); # turn on autoflushing for this file handle
      }

      read_sensors($LOG);
      sleep($read_interval);  
   }
}


# ---------------------------------------------------------------------------
# get current log file name
# ---------------------------------------------------------------------------
sub get_log_name() {

   my ($sec,$min,$hour,$day,$mon,$year,$wday,$yday,$isdst) = localtime;
   $year += 1900;
   $mon = sprintf("%02d", $mon + 1);

   # rotate log at the end of each month
   my $log = $logdir.'/readtemp.'.$year.'-'.$mon.'.log';
}

# ---------------------------------------------------------------------------
# read the sensors and output to file
#
# readtemp returns sensor data as two integers with a newline:
#
#   temp humid\n
#
# our web interface expects the following output per line:
#
#    sensor_id|YYYY-MM-DD HH:MM:SS|temp|humid
# ---------------------------------------------------------------------------
sub read_sensors($) {

   my ($fh) = @_;;

   # read each sensor
   for ( my $i=0; $i < scalar @temp_sensor_pins; $i++ ) {
      my $data = '';
      do {
         $data = `$RT $temp_sensor_pins[$i]`;
         chomp($data);
         if ( $data ) {
            my ($sec,$min,$hour,$day,$mon,$year,$wday,$yday,$isdst) = localtime;
            $year += 1900;
            printf $fh("%d|%d-%02d-%02d %02d:%02d:%02d|%s\n", 
               $i,$year,$mon,$day,$hour,$min,$sec,join('|', split(/\s/, $data)));
         } else {
            # DHT11 sensors only produce data every 2 seconds. If we didn't
            # catch it, try again next cycle
            sleep(3);
         }
      } while ( ! $data );
   }
}

main();

