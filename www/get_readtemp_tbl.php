<?php
   # used in ajax call from index.php to get contents of readtemp log

   $ret = "<table cellspacing='0'>"
      . "<thead>"
      . "<tr><th>Sensor ID</th><th>YYYY-MM-DD HR:MI:SS</th><th>Temp deg C</th><th>% Humidity</th></tr>"
      . "</thead>"
      . "<tbody>";

   $SOURCE = (array_key_exists('source',$_GET)) ? $_GET['source'] : './readtemp.'.date("l").'.log';
   $rows = explode("\n", file_get_contents($SOURCE));
   $rownum = 0;
   foreach ( $rows as $row ) {
      $d = explode('|', $row);

      if ( empty($d[1]) ) { # use col[1] since sensor 0 evaluates as empty
         continue;
      }
      # toggle row color
      $rownum++;
      if ( ($rownum % 2) == 0 ) {
         $class = 'even';
      } else {
         $class = '';
      }
      $ret .= "<tr class='$class'><td>$d[0]</td><td>$d[1]</td><td>$d[2] &deg;C</td><td>$d[3]%</td></tr>";
   }

   $ret .= "</tbody>"
      . "</table>";

   echo $ret;
?> 

