#include "dotstar.h"

#define INTENSITY 5


 dotstar_t;



void dotstar_tx_start()
{

	bcm2835_spi_transfer(0);
	bcm2835_spi_transfer(0);
	bcm2835_spi_transfer(0);
	bcm2835_spi_transfer(0);
}

void dotstar_tx_stop()
{

	bcm2835_spi_transfer(255);
	bcm2835_spi_transfer(255);
	bcm2835_spi_transfer(255);
	bcm2835_spi_transfer(255);
}

void dotstar_set_led(dotstar_t* buffer, uint16_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t i)
{
	dotstar_t t;
	t.r = r;
	t.g = g;
	t.b = b;
	t.i = 0xE0 | i;
	buffer[index] = t;

}

void dotstar_refresh(dotstar_t* buffer, uint16_t len)
{
	dotstar_tx_start();
	uint16_t i = len;
	for (i = 0; i < len; i++)
	{
		bcm2835_spi_transfer(buffer[i].i);
		bcm2835_spi_transfer(buffer[i].r);
		bcm2835_spi_transfer(buffer[i].g);
		bcm2835_spi_transfer(buffer[i].b);

	}
	
	dotstar_tx_stop();
}
dotstar_t* dotstar_init(uint16_t n)
{

	if (!bcm2835_init())
	{
		printf("bcm2835_init failed. Are you running as root??\n");
		return 0;
	}

	if (!bcm2835_spi_begin())
	{
		printf("bcm2835_spi_begin failed. Are you running as root??\n");
		return 0;
	}
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); 
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default

	dotstar_t* ptr = (dotstar_t* )malloc(n*sizeof(dotstar_t));
	return ptr;

}

int dotstar_deinit()
{
	bcm2835_spi_end();
	bcm2835_close();
}
