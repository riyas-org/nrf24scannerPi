#include <spi.h>
#include "RF24_config.h"
// Poor Man's Wireless 2.4GHz Scanner ported for raspberry pi
// Uses nrf24l01 based on routines from rf24 port for raspberry pi
// Credits to the arduino version of poormans scanner and authors of included files 
// read more about the connections at blog.riyas.org
#define CHANNELS  64
int channel[CHANNELS];

// greyscale mapping 
int  line;
char grey[] = " .:-=+*aRW";

// nRF24L01P registers we need
#define _NRF24_CONFIG      0x00
#define _NRF24_EN_AA       0x01
#define _NRF24_RF_CH       0x05
#define _NRF24_RF_SETUP    0x06
#define _NRF24_RPD         0x09
#define byte unsigned char

uint8_t ce_pin =25;
uint8_t csn_pin =8;
uint32_t spispeed=8000000;
SPI spi;

// get the value of a nRF24L01p register
byte getRegister(byte r)
{
  byte c;
  digitalWrite(csn_pin,LOW);
  c = spi.transfer(r&0x1F);
  c = spi.transfer(0);  
  digitalWrite(csn_pin,HIGH);

  return(c);
}

// set the value of a nRF24L01p register
void setRegister(byte r, byte v)
{
  digitalWrite(csn_pin,LOW);
  spi.transfer((r&0x1F)|0x20);
  spi.transfer(v);
  digitalWrite(csn_pin,HIGH);
}
  
// power up the nRF24L01p chip
void powerUp(void)
{
  setRegister(_NRF24_CONFIG,getRegister(_NRF24_CONFIG)|0x02);
  delayMicroseconds(130);
}

// switch nRF24L01p off
void powerDown(void)
{
  setRegister(_NRF24_CONFIG,getRegister(_NRF24_CONFIG)&~0x02);
}

// enable RX 
void enable(void)
{
    digitalWrite(ce_pin,HIGH); 
}

// disable RX
void disable(void)
{
    digitalWrite(ce_pin,LOW); 
}

// setup RX-Mode of nRF24L01p
void setRX(void)
{
  setRegister(_NRF24_CONFIG,getRegister(_NRF24_CONFIG)|0x01);
  enable();
  // this is slightly shorter than
  // the recommended delay of 130 usec
  // - but it works for me and speeds things up a little...
  delayMicroseconds(100);
}

// scanning all channels in the 2.4GHz band
void scanChannels(void)
{
  //printf("scanning\n");
  disable();
  for( int j=0 ; j<100  ; j++)
  {
    //printf(".");
    for( int i=0 ; i<CHANNELS ; i++)
    {
      // select a new channel
      setRegister(_NRF24_RF_CH,(128*i)/CHANNELS);
      
      // switch on RX
      setRX();
      
      // wait enough for RX-things to settle orig 40
      delayMicroseconds(40);
      
      // this is actually the point where the RPD-flag
      // is set, when CE goes low
      disable();
      
      // read out RPD flag; set to 1 if 
      // received power > -64dBm
      if( getRegister(_NRF24_RPD)>0 )   channel[i]++;
    }
  }

}

// outputs channel data as a simple grey map
void outputChannels(void)
{
  int norm = 0;
  // find the maximal count in channel array
  for( int i=0 ; i<CHANNELS ; i++)
    if( channel[i]>norm ) norm = channel[i];
    
  // now output the data
  printf("|");
  for( int i=0 ; i<CHANNELS ; i++)
  {
    int pos;
    
    // calculate grey value position
    if( norm!=0 ) pos = (channel[i]*10)/norm;
    else          pos = 0;
    
    // boost low values
    if( pos==0 && channel[i]>0 ) pos++;
    
    // clamp large values
    if( pos>9 ) pos = 9;
   
    // print it out
    printf("%c",grey[pos]);
    channel[i] = 0;
  }
  
  // indicate overall power
  printf("|");
  printf("%d\n",norm);
}

// give a visual reference between WLAN-channels and displayed data
void printChannels(void)
{
  // output approximate positions of WLAN-channels
  //Serial.println(">      1 2  3 4  5  6 7 8  9 10 11 12 13  14                     <");
  printf(">      1 2  3 4  5  6 7 8  9 10 11 12 13  14                     <\n");
}

void setup()
{
   
  printf("Starting Poor Man's Wireless 2.4GHz Scanner ...\n");
  printf("\n");

  // Channel Layout
  // 0         1         2         3         4         5         6
  // 0123456789012345678901234567890123456789012345678901234567890123
  //       1 2  3 4  5  6 7 8  9 10 11 12 13  14                     | 
  //
  printf("Channel Layout\n");
  printChannels();
  
  // Setup SPI
  spi.setdevice("/dev/spidev0.0");
  spi.setspeed(spispeed);
  spi.setbits(8);
  spi.init();

// Activate Chip Enable
  pinMode(ce_pin,OUTPUT);
  pinMode(csn_pin,OUTPUT);
  digitalWrite(ce_pin,LOW);
  digitalWrite(csn_pin,HIGH);
  disable();
  
  // now start receiver
  powerUp();
  
  // switch off Shockburst
  setRegister(_NRF24_EN_AA,0x0);
  
  // make sure RF-section is set properly 
  // - just write default value... 
  setRegister(_NRF24_RF_SETUP,0x0F); 
  
  // reset line counter
  line = 0;
}

void loop() 
{ 
  // do the scan
  scanChannels();
  // output the result
  outputChannels();
  // output WLAN-channel reference every 12th line
  if( line++>12 )
  {
    printChannels();
    line = 0;
  }
}


int main(int argc, char** argv)
{
        setup();
        while(1)
                loop();

        return 0;
}

