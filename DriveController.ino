#include <string.h>

#define IBUS_BUFFSIZE 32    // Max iBus packet size (2 byte header, 14 channels x 2 bytes, 2 byte checksum)
#define IBUS_MAXCHANNELS 10 // My TX only has 10 channels, no point in polling the rest

static uint8_t ibusIndex = 0;
static uint8_t ibus[IBUS_BUFFSIZE] = {0};
static uint16_t rcValue[IBUS_MAXCHANNELS];

static boolean rxFrameDone;

void setup() 
{
  Serial.begin(115200); 
  Serial.println("setup done!");
}

void loop() 
{
  readRx();
}

void readRx()
{
  rxFrameDone = false;
  
  if (Serial.available())
  {
    uint8_t val = Serial.read();
    // Look for 0x2040 as start of packet
    if (ibusIndex == 0 && val != 0x20)
    {
      ibusIndex = 0;
      return;
    }
    if (ibusIndex == 1 && val != 0x40) 
    {
      ibusIndex = 0;
      return;
    }

    if (ibusIndex == IBUS_BUFFSIZE)
    {
      ibusIndex = 0;
      int high=3;
      int low=2;
      for(int i=0;i<IBUS_MAXCHANNELS; i++)
      {
        //left shift away the first 8 bits of the first byte and add the whole value of the previous one
        rcValue[i] = (ibus[high] << 8) + ibus[low];
        Serial.print(rcValue[i]);
        Serial.print("     ");
        high += 2;
        low += 2;
      }
      Serial.println();
      rxFrameDone = true;
      return;
    }
    else
    {
      ibus[ibusIndex] = val;
      ibusIndex++;
    }
  }
}
