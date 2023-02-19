#include <Arduino.h>
#include <HardwareSerial.h>
#include "VeDirectFrameHandler.hpp"

HardwareSerial SerialPort2(2);
VeDirectFrameHandler VeDirect;


/*
void reatInBuffer()
{    char buffer[512]; 
    size_t length = SerialPort2.read(buffer, 512);

    if(length > 0)
    {
      for(int i = 0; i < length; i++)
        Serial.printf("%c", buffer[i]);
      //Serial.println();
      //Serial.println("++++++++++");
    }
}
void readInString()
{
    String line = SerialPort2.readStringUntil('\n');

    if(line.startsWith("Checksum"))
      Serial.println("+++++++++++++++");
    else
      Serial.printf("%s\n", line);
}
*/

 void setup() {
   Serial.begin(115200);
   SerialPort2.begin(19200, SERIAL_8N1, 16, 17);
   delay(1000);


   Serial.println("");
   Serial.println("+++++ Victron BMV - NEMA200 IO ++++");
   Serial.println("starting....");
 }
 
 long _frameCount = 0;
void loop() {

  if (SerialPort2.available())
  {
    int byte = SerialPort2.read();
    if(byte >= 0)
    {
      VeDirect.RxData(byte);
      long currentFrame = VeDirect.GetFrameCount();

      if(_frameCount < currentFrame)
      {
        Serial.printf("++++++Frame: %4i +++++++++++++\n", currentFrame);

        for(int i = 0; i < VeDirect.veEnd; i++)
        {
          Serial.printf("%s\t%s\n", VeDirect.veName[i], VeDirect.veValue[i]);
        }

        Serial.printf("++++++ endFrame +++++++++++++\n\n");
      }
      _frameCount = currentFrame;
      
    }
  }
}