<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
   <title>server room temp sensor</title>
   <link href="assets/css/simple-little-table.css" rel="stylesheet">
</head>

<body>
   <h1>Server Room Temp Sensors</h1>
   <table cellspacing='0'>
   <thead>
      <tr><th>YYYY-MM-DD HR:MI:SS</th><th>Temp deg C</th><th>% Humidity</th></tr>
   </thead>
   <tbody>
<?php
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
      echo "<tr class='$class'><td>$d[0]</td><td>$d[1] &deg;C</td><td>$d[2]%</td></tr>";
   }
?>      
   </tbody>
   </table>
</body>
</html>

