// MODIFIED FROM CAN Receive Example
//

#include <mcp_can.h>
#include <SPI.h>
#include <FastLED.h>  // maybe a better lib than fastled? something lighter

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];
#define DATA_PIN 3
#define DEVICE_ID 55
#define NUM_LEDS 50
#define COLOR_ORDER RGB
#define LED_TYPE WS2812B
#define CAN0_INT 2  // Set INT to pin 2
MCP_CAN CAN0(10);   // Set CS to pin 10
CRGB leds[NUM_LEDS];
int mode = 0;
int color[] = { 0, 0, 0 };
bool isRainbowing = false;
bool isRainbow1ing = false;

void setup() {
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  if (CAN0.begin(MCP_EXT, CAN_1000KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully!");
    digitalWrite(4, HIGH);
  } else {
    Serial.println("Error Initializing MCP2515...");
  }

  CAN0.init_Mask(0, 1, 0x3F);  // make sure these guys are doing something -- test on prebuilt modules
  CAN0.init_Filt(0, 1, 0x37);
  CAN0.init_Filt(1, 1, 0x37);
  CAN0.init_Filt(2, 1, 0x37);
  CAN0.init_Filt(3, 1, 0x37);
  CAN0.init_Mask(1, 1, 0x3F);
  CAN0.init_Filt(4, 1, 0x37);
  CAN0.init_Filt(5, 1, 0x37);
  // maybe incorporate other parts of the CAN packet in choosing patterns (API ID)
  CAN0.setMode(MCP_NORMAL);  // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);  // Configuring pin for /INT input

  Serial.println("MCP2515 Library Receive Example...");
}

void loop() {
  // if (mode == 7) {
  //   fill_solid(leds, NUM_LEDS, CRGB(color[0], color[1], color[2]));
  //   FastLED.show();
  // } else {
  //   rainbow();  // find some way to let other patterns run -- lower priority
  // }
  //mode = 3;
  switch (mode) {
    case 1:
      {
        fill_solid(leds, NUM_LEDS, CRGB(color[0], color[1], color[2]));
        FastLED.show();
        break;
      }
    case 2:
      {
        if (!isRainbowing) {
          isRainbow1ing = false;
          rainbow();
        }
        break;
      }
    case 3:
      {
        if (!isRainbow1ing) {
          isRainbowing = false;
          rainbowCycle1(10, 10, 10);
        }
        break;
      }
  }
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);

  if (!digitalRead(CAN0_INT))  // If CAN0_INT pin is low, read receive buffer
  {
    digitalWrite(4, HIGH);
    CAN0.readMsgBuf(&rxId, &len, rxBuf);  // Read data: len = data length, buf = data byte(s)

    if ((rxId & 0x3F) == DEVICE_ID) {
      mode = rxBuf[0];
      if (mode == 1) {
        color[0] = rxBuf[1];  // probably a more elegant way to do this -  make sure ordering is correct?
        color[1] = rxBuf[2];
        color[2] = rxBuf[3];
      }
    }
  }
  delay(10);  // is this delay necessary? should it be shorter?
}

// MODIFIED FROM https://github.com/leon-anavi/arduino-fastled-rainbow/blob/master/fastled_rainbow/fastled_rainbow.ino

void rainbow() {
  isRainbowing = true;
  randomSeed(millis());

  int wait = random(10, 30);
  int dim = random(4, 6);
  int max_cycles = 8;
  int cycles = random(1, max_cycles + 1);

  rainbowCycle(wait, cycles, dim);
  isRainbowing = false;
}

void rainbowCycle(int wait, int cycles, int dim) {
  //loop several times with same configurations and same delay
  for (int cycle = 0; cycle < cycles; cycle++) {
    byte dir = random(0, 2);
    int k = 255;

    //loop through all colors in the wheel
    for (int j = 0; j < 256; j++, k--) {

      if (k < 0) {
        k = 255;
      }

      //Set RGB color of each LED
      for (int i = 0; i < NUM_LEDS; i++) {
        CRGB ledColor = wheel(((i * 256 / NUM_LEDS) + (dir == 0 ? j : k)) % 256, dim);
        leds[i] = ledColor;
      }

      FastLED.show();
      FastLED.delay(wait);
    }
  }
}

CRGB wheel(int WheelPos, int dim) {
  CRGB color;
  if (85 > WheelPos) {
    color.r = 0;
    color.g = WheelPos * 3 / dim;
    color.b = (255 - WheelPos * 3) / dim;
    ;
  } else if (170 > WheelPos) {
    color.r = WheelPos * 3 / dim;
    color.g = (255 - WheelPos * 3) / dim;
    color.b = 0;
  } else {
    color.r = (255 - WheelPos * 3) / dim;
    color.g = 0;
    color.b = WheelPos * 3 / dim;
  }
  return color;
}

void rainbowCycle1(int wait, int cycles, int dim) {
  isRainbow1ing = true;
  Serial.println("Let's make a rainbow.");
  //loop several times with same configurations and same delay
  unsigned int rgbColour[3];

  // Start off with red.
  rgbColour[0] = 255;
  rgbColour[1] = 0;
  rgbColour[2] = 0;

  // Choose the colours to increment and decrement.
  for (int decColour = 0; decColour < 3; decColour += 1) {
    int incColour = decColour == 2 ? 0 : decColour + 1;

    // cross-fade the two colours.
    for (int i = 0; i < 255; i += 1) {
      rgbColour[decColour] -= 1;
      rgbColour[incColour] += 1;

      fill_solid(leds, NUM_LEDS, CRGB(rgbColour[0], rgbColour[1], rgbColour[2]));

      FastLED.show();
      FastLED.delay(wait);
    }
  }
  isRainbow1ing = false;
}

/*********************************************************************************************************
  END FILE
*******************************************************************************************/