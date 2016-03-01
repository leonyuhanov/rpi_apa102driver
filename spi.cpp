// spi.cpp

#include <bcm2835.h>
#include <stdio.h>
#include <math.h>

//Set up Data Block for LEDs
const uint8_t bytesPerLED = 4;
const uint8_t numOfLEDs = 144;
const int16_t endFrameLength =  9;	//round( (numOfLEDs/2)/8 );
const int16_t spiFrameLength = 620;	//(2+numOfLEDs+endFrameLength)*bytesPerLED;
char ledDataBlock[spiFrameLength];

//colour data
uint8_t tmpColour[3] = {0,0,0};
uint8_t maxValue = 128;
int16_t rainbowSize = maxValue*6;

void initData()
{
	int16_t ledIndex = 0;
	
	//Init the start Frame
	ledDataBlock[0] = 0;
	ledDataBlock[1] = 0;
	ledDataBlock[2] = 0;
	ledDataBlock[3] = 0;
	//Init the Driver LED
	ledDataBlock[4] = 255;
	ledDataBlock[5] = 0;
	ledDataBlock[6] = 0;
	ledDataBlock[7] = 0;
	//init each LED
	for(ledIndex=8; ledIndex<spiFrameLength-(endFrameLength*bytesPerLED); ledIndex+=bytesPerLED)
	{
		ledDataBlock[ledIndex] = 255;
		ledDataBlock[ledIndex+1] = 0;
		ledDataBlock[ledIndex+2] = 0;
		ledDataBlock[ledIndex+3] = 0;
	}
	//init the end frame
	for(ledIndex; ledIndex<spiFrameLength; ledIndex+=bytesPerLED)
	{
		ledDataBlock[ledIndex] = 255;
		ledDataBlock[ledIndex+1] = 255;
		ledDataBlock[ledIndex+2] = 255;
		ledDataBlock[ledIndex+3] = 255;
	}
	return;
}

void getColour(int16_t colourIndex, uint8_t *currentColour)
{

 if( colourIndex>=0 && colourIndex<maxValue )
 {
    currentColour[0] = maxValue;
    currentColour[1] = colourIndex;
    currentColour[2] = 0; 
 }
 else if(colourIndex>=maxValue && colourIndex<maxValue*2)
 {
    currentColour[0] = maxValue-(colourIndex-maxValue);
    currentColour[1] = maxValue;
    currentColour[2] = 0; 
 }
 else if(colourIndex>=maxValue*2 && colourIndex<maxValue*3)
 {
    currentColour[0] = 0;
    currentColour[1] = maxValue;
    currentColour[2] = colourIndex-maxValue*2; 
 }
 else if(colourIndex>=maxValue*3 && colourIndex<maxValue*4)
 {
    currentColour[0] = 0;
    currentColour[1] = maxValue-(colourIndex-maxValue*3);
    currentColour[2] = maxValue; 
 }
 else if(colourIndex>=maxValue*4 && colourIndex<maxValue*5)
 {
    currentColour[0] = colourIndex-maxValue*4;
    currentColour[1] = colourIndex-maxValue*4;
    currentColour[2] = maxValue; 
 }
 else if(colourIndex>=maxValue*5 && colourIndex<maxValue*6)
 {
    currentColour[0] = maxValue;
    currentColour[1] = maxValue-(colourIndex-maxValue*5);
    currentColour[2] = maxValue-(colourIndex-maxValue*5); 
 }
 return;
}

int main(int argc, char **argv)
{
    int16_t ledIndexCounter = 0, scanPasses=0, colourIndex=0;
	
	initData();
	
	//Initiate the SPI Data Frame
	if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }
    if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failedg. Are you running as root??\n");
      return 1;
    }
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
	/*
		
	BCM2835_SPI_MODE0 		CPOL = 0, CPHA = 0
	BCM2835_SPI_MODE1 		CPOL = 0, CPHA = 1
	BCM2835_SPI_MODE2 		CPOL = 1, CPHA = 0
	BCM2835_SPI_MODE3 		CPOL = 1, CPHA = 1	
	*/
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16); // The default
	/*

		BCM2835_SPI_CLOCK_DIVIDER_65536 	65536 = 262.144us = 3.814697260kHz
		BCM2835_SPI_CLOCK_DIVIDER_32768 	32768 = 131.072us = 7.629394531kHz
		BCM2835_SPI_CLOCK_DIVIDER_16384 	16384 = 65.536us = 15.25878906kHz
		BCM2835_SPI_CLOCK_DIVIDER_8192 		8192 = 32.768us = 30/51757813kHz
		BCM2835_SPI_CLOCK_DIVIDER_4096 		4096 = 16.384us = 61.03515625kHz
		BCM2835_SPI_CLOCK_DIVIDER_2048 		2048 = 8.192us = 122.0703125kHz
		BCM2835_SPI_CLOCK_DIVIDER_1024 		1024 = 4.096us = 244.140625kHz
		BCM2835_SPI_CLOCK_DIVIDER_512 		512 = 2.048us = 488.28125kHz
		BCM2835_SPI_CLOCK_DIVIDER_256 		256 = 1.024us = 976.5625kHz
		BCM2835_SPI_CLOCK_DIVIDER_128 		128 = 512ns = = 1.953125MHz
		BCM2835_SPI_CLOCK_DIVIDER_64 		64 = 256ns = 3.90625MHz
		BCM2835_SPI_CLOCK_DIVIDER_32 		32 = 128ns = 7.8125MHz
		BCM2835_SPI_CLOCK_DIVIDER_16 		16 = 64ns = 15.625MHz
		BCM2835_SPI_CLOCK_DIVIDER_8 		8 = 32ns = 31.25MHz
		BCM2835_SPI_CLOCK_DIVIDER_4 		4 = 16ns = 62.5MHz
		BCM2835_SPI_CLOCK_DIVIDER_2 		2 = 8ns = 125MHz, fastest you can get	
	
	*/
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
    
	for(colourIndex=0; colourIndex<rainbowSize*10; colourIndex+=5)
	{
		getColour(colourIndex%rainbowSize, tmpColour);
		//set 1st Pixel
		ledDataBlock[8]=255;
		ledDataBlock[9]=tmpColour[2];
		ledDataBlock[10]=tmpColour[1];
		ledDataBlock[11]=tmpColour[0];
		//push the array
		for(ledIndexCounter=(spiFrameLength-(endFrameLength*bytesPerLED))-bytesPerLED; ledIndexCounter>8; ledIndexCounter-=bytesPerLED)
		{
			ledDataBlock[ledIndexCounter] = ledDataBlock[ledIndexCounter-bytesPerLED];
			ledDataBlock[ledIndexCounter+1] = ledDataBlock[ledIndexCounter+1-bytesPerLED];
			ledDataBlock[ledIndexCounter+2] = ledDataBlock[ledIndexCounter+2-bytesPerLED];
			ledDataBlock[ledIndexCounter+3] = ledDataBlock[ledIndexCounter+3-bytesPerLED];
		}
		//send to LEDs
		bcm2835_spi_writenb(ledDataBlock, spiFrameLength);
		bcm2835_delay(5);
	}
    //clear and render
	initData();
	bcm2835_spi_writenb(ledDataBlock, spiFrameLength);
	//close the spi bus
	bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
