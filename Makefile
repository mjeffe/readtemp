# ---------------------------------------------------------------------------
# $Id$
#
# See http://learn.adafruit.com/dht-humidity-sensing-on-raspberry-pi-with-gdocs-logging/software-install
# ---------------------------------------------------------------------------
CC = gcc
#CFLAGS =  -std=gnu99 -I. -lbcm2835
CFLAGS =  -std=c99 -I. -lbcm2835
DEPS = 
OBJ = readtemp.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

readtemp: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

install:
	@cp readtemp readtempd /usr/local/bin/
