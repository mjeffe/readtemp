# readtemp

Read temperature and humidity values from DHT11 sensor connected to a raspberry pi

__Disclaimer:__ I have not taken the time to bundle this for public consumption
and I don't intend to support it.  This was a proof of concept project, and
therefore does not have a nice installer, tests, or any of the other stuff you
would expect. I'm merely making it public in the hope that someone may find
some bits of it useful.

## DESCRIPTION

This little project was created to publish my server closet temperature on an
intranet web server. It uses a DHT11 temp sensor connected to a raspberry pi,
running a web server and php.

readtemp is largely based on the [Adafruit arduino version](https://github.com/adafruit/Adafruit-Raspberry-Pi-Python-Code)

It depends on the [bcm2835 library](http://www.airspayce.com/mikem/bcm2835/index.html) for reading GPIO pins on RPi.

The web publishing stuff uses [Hicharts](https://www.highcharts.com/) to display the data in a nice graph.

## LICENSE

This project is free software. You can redistribute it and/or modify it under the
terms of the FreeBSD License. See header in main `readtemp.c` file.


