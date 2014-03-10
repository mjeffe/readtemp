<?php
   # used in ajax call from index.php to get contents of readtemp log

   $ret = "<table cellspacing='0'>"
      . "<thead>"
      . "<tr><th>YYYY-MM-DD HR:MI:SS</th><th>Temp deg C</th><th>% Humidity</th></tr>"
      . "</thead>"
      . "<tbody>";

   $data = file_get_contents('./readtemp.log');
   $rows = explode("\n", $data);
   $rownum = 0;
   foreach ( $rows as $row ) {
      $d = explode('|', $row);

      if ( empty($d[0]) ) {
         continue;
      }
      # toggle row color
      $rownum++;
      if ( ($rownum % 2) == 0 ) {
         $class = 'even';
      } else {
         $class = '';
      }
      $ret .= "<tr class='$class'><td>$d[0]</td><td>$d[1] &deg;C</td><td>$d[2]%</td></tr>";
   }

   $ret .= "</tbody>"
      . "</table>";

   echo $ret;
?> 

