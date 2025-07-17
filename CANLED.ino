// CAN Receive Example
//

#include <mcp_can.h>
#include <SPI.h>
#include <FastLED.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];
#define DATA_PIN 3
#define DEVICE_ID 55                        
#define NUM_LEDS 30
#define COLOR_ORDER RGB
#define LED_TYPE WS2812B
#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10
CRGB leds[NUM_LEDS];

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_1000KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
  
  Serial.println("MCP2515 Library Receive Example...");
}

void loop()
{
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    
    // if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
    //   //sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    // else
    //   //sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    //Serial.print(msgString);
//if(rxId & 0xFFF80000)==0x0A080000){}
if((rxId&0x3F)==DEVICE_ID){
    int api = rxId & 0x0000FFC0;
    int color =0;
    api = api >> 6;
    Serial.println("DEVICE ACTIVATED");
    Serial.println(api);// RGB ordering!
    for(byte i = 0; i<NUM_LEDS;i++){ 
    leds[i] = CRGB(rxBuf[0],rxBuf[1],rxBuf[2]);}
    FastLED.show();
     for(byte i = 0; i<len; i++){
         sprintf(msgString, " 0x%.2X", rxBuf[i]);
         Serial.print(msgString);

       }
     
    //if(api == 384){ 
    // color+= 16<<rxBuf[0];
    // color+= 8<<rxBuf[1];
    // color+=rxBuf[2];
    // Serial.println(color);
    }
    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);}
    // } else {
    //   for(byte i = 0; i<len; i++){
    //     sprintf(msgString, " 0x%.2X", rxBuf[i]);
    //     Serial.print(msgString);
    //   }
    // }
        
    //Serial.println();
  }
}

/*********************************************************************************************************
  END FILE
*******************************************************************************************/
