#ifndef DOTSTAR_H_
#define DOTSTAR_H_

#include <bcm2835.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct st_dotstar
{
	uint8_t i;
	uint8_t r;
	uint8_t g;
	uint8_t b;
} dotstar_t;



void dotstar_tx_start();

void dotstar_tx_stop();

void dotstar_set_led(dotstar_t* buffer, uint16_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t i);

void dotstar_refresh(dotstar_t* buffer, uint16_t len);
dotstar_t* dotstar_init(uint16_t n);

int dotstar_deinit();

#endif
