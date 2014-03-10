<?php
   # used in ajax call from index.php to get readtemp log as Highchart graph

   
   $FARENHEIT = (array_key_exists('f',$_GET)) ? 1 : 0;
   $temps = array();
   $humid = array();
   $rows = explode("\n", file_get_contents('./readtemp.'.date("l").'.log'));
   $rownum = 0;
   foreach ( $rows as $row ) {
      $d = explode('|', $row);
      if ( empty($d[1]) ) {  # use $d[1] because first field may have 0
         continue;
      }

      $tmp = explode(' ',$d[1]);
      $dt = array_merge(explode('-',$tmp[0]),explode(':',$tmp[1]));
      $dt[1] -= 1;   # javascript months start at 0 not 1. retarded...

      # two dimensional array - first dimension is sensor #
      $t = $d[2];
      if ( $FARENHEIT ) {
         $t = $d[2]*9/5+32;
      }
      $temps[$d[0]][] = '[Date.UTC('.implode(',',$dt).'),'.$t.']';
      $humid[$d[0]][] = '[Date.UTC('.implode(',',$dt).'),'.$d[3].']';

   }

?> 

<script>
            $(function () {
              $('#temperature_chart').highcharts({
                  chart: {
                     type: 'spline',
                     zoomType: 'xy'
                     //,spacingRight: 20
                  },
                  title: {
                      text: 'ARC Server Room Temperature and Humidity'
                  },
                  subtitle: {
                     text: 'Source: <?= '/var/www/pi/readtemp.'.date("l").'.log' ?>'
                  },
                  xAxis: {
                     type: 'datetime',
/*
                     labels: {
                        rotation: -90, 
                        step: 4
                     }
*/
                  },
                  yAxis: [{ // primary axis, left side
                      title: {
                         text: 'Temperature',
                         style: {
                            color: '#8A0829'
                         }
                      },
                      labels: {
                         formatter: function() {
                            if ( <?= $FARENHEIT ?> ) {
                               return this.value + '°F';
                            }
                            return this.value + '°C';
                         },
                         style: {
                            color: '#8A0829'
                         }
                      }
                  }, { // secondary axis, right side
                      gridLineWidth: 0,
                      title: {
                         text: 'Humidity',
                         style: {
                            color: '#0431B4'
                         }
                      },
                      labels: {
                         formatter: function() {
                            return this.value + '%';
                         },
                         style: {
                            color: '#0431B4'
                         }
                      },
                      opposite: true
                  }],
                  tooltip: {
                      enabled: true,
                      formatter: function() {
                         var symbol = '°C';
                         if ( <?= $FARENHEIT ?> ) {
                           symbol = '°F';
                         }
                         if ( this.series.name.match(/Humidity/i) ) {
                            symbol = '%';
                         }
                          return '<b>'+ this.series.name +': '+this.y+symbol+'</b><br/>'
                             + Highcharts.dateFormat('%b %d, %Y %H:%M:%S', this.x);
                      }
                  },
                  plotOptions: {
                      spline: {
                          dataLabels: {
                              enabled: false,
                              style: {
                                  textShadow: '0 0 3px white, 0 0 3px white'
                           }
                         },
                         enableMouseTracking: true,
                         lineWidth: 1,
                         marker: {
                            enabled: false,
                         }
                         /*,
                         pointInterval: 4
                         pointStart: Date.UTC(2009, 9, 6, 0, 0, 0)
                          */
                      }
                  },
                  series: [
                     {
                         name: 'Temperature Sensor 0',
                         color: '#8A0829',
                         yAxis: 0,
                         data: [<?= implode(',', $temps[0]); ?>]
                     }, {
                         name: 'Humidity Sensor 0',
                         color: '#0431B4',
                         yAxis: 1,
                         data: [<?= implode(',', $humid[0]); ?>]
                     }, {
                         name: 'Temperature Sensor 1',
                         color: '#8A0829',
                         yAxis: 0,
                         data: [<?= implode(',', $temps[1]); ?>]
                     }, {
                         name: 'Humidity Sensor 1',
                         color: '#0431B4',
                         yAxis: 1,
                         data: [<?= implode(',', $humid[1]); ?>]
                     }
                  ]
              });
          });

</script>

