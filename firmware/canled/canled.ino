/*
 * CANLED ATmega328P firmware
 *
 * Author: Conor Kelly Gerakos
 * Team: FRC 2404
 * License: GPL v3.0
 * Year: 2025
 */
#include <mcp_can.h>
#include <SPI.h>
#include <FastLED.h>

// CAN Pins
#define CAN0_CS_PIN    10
#define CAN0_INT_PIN   2

// Board LED Pins
#define LED_DATA_PIN   3
#define LED_OK_PIN     4
#define LED_STATUS_PIN 5

// FRC Device ID
#define DEVICE_ID 55

// LED Constants
#define NUM_LEDS 86
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB  // GRB here but actually RGB?

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];
int mode = 0;
int color[] = { 0, 0, 0 };
byte dir;

MCP_CAN CAN0(CAN0_CS_PIN);
CRGB leds[NUM_LEDS];

struct rainbow_state {
  u8 index;
  const u16 wait;
  const u8 dim;
};

struct solid_rainbow_state {
  u8 color;
};

struct real_rainbow_state {
  u8 color;
};

#define SOLID_RAINBOW_INIT() (struct solid_rainbow_state) {.color = 0}
#define RAINBOW_INIT(_wait, _dim) (struct rainbow_state) {.index = 0, .wait = _wait, .dim = _dim}

struct rainbow_state rainbow_one = RAINBOW_INIT(10, 6);
struct solid_rainbow_state rainbow_two = {0};
struct real_rainbow_state rainbow_three = {0};

void setup() {
  pinMode(CAN0_INT_PIN, INPUT);
  pinMode(LED_OK_PIN, OUTPUT);
  pinMode(LED_STATUS_PIN, OUTPUT);
  digitalWrite(LED_STATUS_PIN, LOW);

  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  if (CAN0.begin(MCP_EXT, CAN_1000KBPS, MCP_8MHZ) == CAN_OK) {
    digitalWrite(LED_OK_PIN, HIGH);
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
}

void loop() {
  switch (mode) {
    case 1:
      {
        fill_solid(leds, NUM_LEDS, CRGB(color[0], color[1], color[2]));
        FastLED.show();
        break;
      }
    case 2:
      {
        rainbowCycle(&rainbow_one);
        dir = random(0, 2);
        break;
      }
    case 3:
      {
        rainbowCycleSolid(&rainbow_two, color[0]);
        break;
      }
    case 4:
      {
        rainbow_real(&rainbow_three, color[0]);
        break;
      }
    default:
      {
        break;
      }
  }
  digitalWrite(LED_STATUS_PIN, LOW);

  if (!digitalRead(CAN0_INT_PIN))  // If CAN0_INT_PIN pin is low, read receive buffer
  {
    digitalWrite(LED_OK_PIN, HIGH);
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

void rainbowCycle(struct rainbow_state* state) {
  //loop through all colors in the wheel
  state->index++;
  // if (state->index < 0) {
  //   state->index = 255;
  // }

  //Set RGB color of each LED
  for (int i = 0; i < NUM_LEDS; i++) {
    CRGB ledColor = wheel(((i * 256 / NUM_LEDS) + state->index) % 256, state->dim);
    leds[i] = ledColor;
  }

  FastLED.show();
  FastLED.delay(state->wait);
}


CRGB wheel(int WheelPos, int dim) {
//   CRGB color;  // orgininally R G B ordering
//   if (85 > WheelPos) {
//     color.b = 0;
//     color.g = WheelPos * 3 / dim;
//     color.r = (255 - WheelPos * 3) / dim;
//   } else if (170 > WheelPos) {
//     color.b = WheelPos * 3 / dim;
//     color.g = (255 - WheelPos * 3) / dim;
//     color.r = 0;
//   } else {
//     color.b = (255 - WheelPos * 3) / dim;
//     color.g = 0;
//     color.r = WheelPos * 3 / dim;  // *3
//   }
//   return color;
 CRGB color;
  if (85 > WheelPos)
  {
   color.r=0;
   color.g=WheelPos * 3/dim;
   color.b=(255 - WheelPos * 3)/dim;;
  } 
  else if (170 > WheelPos)
  {
   color.r=WheelPos * 3/dim;
   color.g=(255 - WheelPos * 3)/dim;
   color.b=0;
  }
  else
  {
   color.r=(255 - WheelPos * 3)/dim;
   color.g=0;
   color.b=WheelPos * 3/dim;
  }
  return color;
 }


void rainbowCycleSolid(struct solid_rainbow_state* state, u8 wait) {
  /* TODO: i dont know what the diffrence between the conversion functions are */
  CRGB c = hsv2rgb_rainbow(CHSV(state->color, 255, 255));
  state->color++;
  fill_solid(leds, NUM_LEDS, c);\
  FastLED.show();
  FastLED.delay(wait);
  // state->num_color++;
  // if (state->num_color == 0) {
  //   state->num_color = 0;

  //   state->decrease++;
  //   if (state->decrease > 3) {
  //     state->decrease = 0;
  //   }

  //   state->increase = state->decrease == 2 ? 0 : state->decrease + 1;
  // }

  // state->color[state->decrease]--;
  // state->color[state->increase]++;

  // fill_solid(leds, NUM_LEDS, CRGB(state->color[0], state->color[1], state->color[2]));

  // FastLED.show();
  // FastLED.delay(state->wait);
}

void rainbow_real(struct real_rainbow_state* state, u8 wait){
  state->color++;
  for(int i = 0; i<NUM_LEDS;i++){
    leds[i].setHSV(state->color+i,255,255);
  }
  FastLED.show();
  FastLED.delay(wait);
}
