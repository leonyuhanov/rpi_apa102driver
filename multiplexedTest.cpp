//g++ -o wowsuit wowsuit.cpp apa102LEDStrip.cpp -l bcm2835

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h> 
#include <math.h>

#define MPLEXPIN0 RPI_BPLUS_GPIO_J8_29
#define MPLEXPIN1 RPI_BPLUS_GPIO_J8_31
#define MPLEXPIN2 RPI_BPLUS_GPIO_J8_33
#define MPLEXPIN3 RPI_BPLUS_GPIO_J8_35

//LED Data
const short int numLeds = 13;
const uint8_t bytesPerLED = 4;
const short int endFrames = round( (((numLeds)/2)/8)*bytesPerLED ) ;
const short int frameLength = ((1+numLeds)*bytesPerLED)+endFrames; // (1 Start Frame) + (1 Drive LED Frame) + numLEDs + round(((numLeds+1)/2)/8)
char stripOne[frameLength], stripTwo[frameLength];              //Array storing ALL leds data including Start Frame+driver frame+LEDs+end frames

//Multiplexed Data Blocks
uint8_t multiplexerAddress[4] = {0,0,0,0};

short int ledIndex=0;
uint8_t maxValue=128;                    //Maximum value to be used PER COLOUR on each Pixel
uint8_t maxBrightness = 240;            //Maximum Globbal Brightness Level (range is 225-254)
short int colIndex=0;                //Used to store compressed colour index
uint8_t currentCol[3] = {0,0,0};        //Colour array
short int rainbowSize = maxValue*6;  //Maximum number of individual colours based on the maxValue


void setMultiplexerAddress(uint8_t mAddress)
{
	switch(mAddress)
	{
		case	0:	multiplexerAddress[0] = 0;
					multiplexerAddress[1] = 0;
					multiplexerAddress[2] = 0;
					multiplexerAddress[3] = 0;
					break;
		case	1:	multiplexerAddress[0] = 1;
					multiplexerAddress[1] = 0;
					multiplexerAddress[2] = 0;
					multiplexerAddress[3] = 0;
					break;
		case	2:	multiplexerAddress[0] = 0;
					multiplexerAddress[1] = 1;
					multiplexerAddress[2] = 0;
					multiplexerAddress[3] = 0;
					break;
		case	3:	multiplexerAddress[0] = 1;
					multiplexerAddress[1] = 1;
					multiplexerAddress[2] = 0;
					multiplexerAddress[3] = 0;
					break;
		case	4:	multiplexerAddress[0] = 0;
					multiplexerAddress[1] = 0;
					multiplexerAddress[2] = 1;
					multiplexerAddress[3] = 0;
					break;
		case	5:	multiplexerAddress[0] = 1;
					multiplexerAddress[1] = 0;
					multiplexerAddress[2] = 1;
					multiplexerAddress[3] = 0;
					break;
		case	6:	multiplexerAddress[0] = 0;
					multiplexerAddress[1] = 1;
					multiplexerAddress[2] = 1;
					multiplexerAddress[3] = 0;
					break;
		case	7:	multiplexerAddress[0] = 1;
					multiplexerAddress[1] = 1;
					multiplexerAddress[2] = 1;
					multiplexerAddress[3] = 0;
					break;
		case	8:	multiplexerAddress[0] = 0;
					multiplexerAddress[1] = 0;
					multiplexerAddress[2] = 0;
					multiplexerAddress[3] = 1;
					break;
		case	9:	multiplexerAddress[0] = 1;
					multiplexerAddress[1] = 0;
					multiplexerAddress[2] = 0;
					multiplexerAddress[3] = 1;
					break;
		default:	break;
	}
	bcm2835_gpio_write(MPLEXPIN0, multiplexerAddress[0]);
	bcm2835_gpio_write(MPLEXPIN1, multiplexerAddress[1]);
	bcm2835_gpio_write(MPLEXPIN2, multiplexerAddress[2]);
	bcm2835_gpio_write(MPLEXPIN3, multiplexerAddress[3]);
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

void renderLEDs()
{
    short int LEDCounter = 0;
    setMultiplexerAddress(0);
    bcm2835_spi_writenb(stripOne, frameLength);
    setMultiplexerAddress(1);
    bcm2835_spi_writenb(stripTwo, frameLength);
}

int main(int argc, char **argv)
{	
  //Initialize the Start Frames
  for(ledIndex=0; ledIndex<4; ledIndex++)
  {
    stripOne[ledIndex] = 0;
    stripTwo[ledIndex] = 0;
  }
  //Clear out the LED data Frames
  for(ledIndex=bytesPerLED; ledIndex<frameLength-endFrames; ledIndex+=bytesPerLED)
  {
    stripOne[ledIndex] = maxBrightness;
    stripTwo[ledIndex] = maxBrightness;
    stripOne[ledIndex+1] = 0;
    stripTwo[ledIndex+1] = 0;
    stripOne[ledIndex+2] = 0;
    stripTwo[ledIndex+2] = 0;
    stripOne[ledIndex+3] = 0;
    stripTwo[ledIndex+3] = 0;
  }
  //Initialize the end frames
  for(ledIndex=frameLength-endFrames; ledIndex<frameLength; ledIndex+=bytesPerLED)
  {
    stripOne[ledIndex] = maxBrightness;
    stripTwo[ledIndex] = maxBrightness;
    stripOne[ledIndex+1] = 0;
    stripTwo[ledIndex+1] = 0;
    stripOne[ledIndex+2] = 0;
    stripTwo[ledIndex+2] = 0;
    stripOne[ledIndex+3] = 0;
    stripTwo[ledIndex+3] = 0;
  }

	//Initiate the SPI device
	if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }
    if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failed. Are you running as root??\n");
      return 1;
    }
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(250016000/20000000); // The default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
    
	//Set up the multiplexer pins
	bcm2835_gpio_fsel(MPLEXPIN0, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(MPLEXPIN1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(MPLEXPIN2, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(MPLEXPIN3, BCM2835_GPIO_FSEL_OUTP);
	setMultiplexerAddress(0);
	
	//do stuff
	while(1)
	{
	  getColour(colIndex, currentCol);
	  //set LED 1(the 1st visible LED not the driver LED) to to the colour calulated above
	  stripOne[bytesPerLED+1] = currentCol[2];
	  stripOne[bytesPerLED+2] = currentCol[1];
	  stripOne[bytesPerLED+3] = currentCol[0];
	  //Push the array colour data inwards
	  //eg [r][0][0] will be [r][r][0] data is pushed RIGHT
	  for(ledIndex=frameLength-bytesPerLED; ledIndex>4; ledIndex-=bytesPerLED)
	  {
		stripOne[ledIndex+1] = stripOne[ledIndex-bytesPerLED+1];
		stripOne[ledIndex+2] = stripOne[ledIndex-bytesPerLED+2];
		stripOne[ledIndex+3] = stripOne[ledIndex-bytesPerLED+3];
	  }

	  stripTwo[frameLength-bytesPerLED+1] = currentCol[2];
	  stripTwo[frameLength-bytesPerLED+2] = currentCol[1];
	  stripTwo[frameLength-bytesPerLED+3] = currentCol[0];

	  for(ledIndex=bytesPerLED; ledIndex<frameLength-bytesPerLED; ledIndex+=bytesPerLED)
	  {
		stripTwo[ledIndex+1] = stripTwo[ledIndex+bytesPerLED+1];
		stripTwo[ledIndex+2] = stripTwo[ledIndex+bytesPerLED+2];
		stripTwo[ledIndex+3] = stripTwo[ledIndex+bytesPerLED+3];
	  }

	  //check if colIndex greater than or equal to the maximum volour range value if so reset it to 0 else increment ot get next colour
	  if(colIndex>=rainbowSize)
	  {
		 colIndex=0;
	  }
	  else
	  {
		colIndex+=20;
	  } 
	  //renred LED data to LED strip
	  renderLEDs();
	  bcm2835_delay(50);
	}
	//end do stuff

	
	bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
