# readtemp

Read temperature and humidity values from DHT11 sensor connected to a raspberry pi

## DESCRIPTION

This little project was created to publish my server closet temperature on an
internal web server. It uses a DHT11 temp sensor connected to a raspberry pi,
running a web server and php.

readtemp is largely based on the [Adafruit arduino version](https://github.com/adafruit/Adafruit-Raspberry-Pi-Python-Code)

It depends on the [bcm2835 library](http://www.airspayce.com/mikem/bcm2835/index.html) for reading GPIO pins on RPi.

The web publishing stuff uses [Hicharts](https://www.highcharts.com/) to display the data in a nice graph.

