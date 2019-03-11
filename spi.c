// spi.c
//
// Example program for bcm2835 library
// Shows how to interface with SPI to transfer a byte to and from an SPI device
//
// After installing bcm2835, you can build this 
// with something like:
// gcc -o spi spi.c -l bcm2835
// sudo ./spi
//
// Or you can test it before installing with:
// gcc -o spi -I ../../src ../../src/bcm2835.c spi.c
// sudo ./spi
//
// Author: Mike McCauley
// Copyright (C) 2012 Mike McCauley
// $Id: RF22.h,v 1.21 2012/05/30 01:51:25 mikem Exp $

#include <stdio.h>
#include <stdlib.h>
#include "dotstar.h"

int main(int argc, char **argv)
{
	// If you call this, it will not actually access the GPIO
	// Use for testing
	//        bcm2835_set_debug(1);
	
	dotstar_t* led_array = dotstar_init(144);	
	if (! led_array)
		return 1;
	int i;
	for (i = 0; i < 144; i++)
	{
		dotstar_set_led(led_array, i, 255/144*i, 255/144*(20-i), 124, INTENSITY);
	}
	dotstar_refresh(led_array, 144);


	return 0;
}

