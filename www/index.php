<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
   <title>server room temp sensor</title>
   <link href="assets/css/simple-little-table.css" rel="stylesheet">
   <link href="assets/css/readtemp.css" rel="stylesheet">
</head>

<body>

   <div class="container">

      <div id="temperature_chart" class="chart"></div>
      <hr />
      <div id="readtemp_log"></div>

      <script src="/pi/assets/js/jquery.min.js"></script>
      <script src="/pi/assets/js/highcharts.js"></script>
      <script type="text/javascript">
<?php 
$qstr = ( empty($_SERVER['QUERY_STRING']) ) ? '' : '?'.$_SERVER['QUERY_STRING'];
?>
         function chart_temperature() {
            var p = {};
            $('#temperature_chart').load('/pi/get_readtemp_hc.php<?= $qstr ?>', p, function(str) {});
         }

         function get_readtemp_tbl() {
            var p = {};
            $('#readtemp_log').load('/pi/get_readtemp_tbl.php<?= $qstr ?>', p, function(str) {});
         }

         $(document).ready(function(){
<?php if ( array_key_exists('t',$_GET) ) { ?>
            get_readtemp_tbl();
            setInterval("get_readtemp_tbl()", 60000);
<?php } ?>
            chart_temperature();
            setInterval("chart_temperature()", 60000);
         });

      </script>
   </div>  <!-- container -->
</body>
</html>

