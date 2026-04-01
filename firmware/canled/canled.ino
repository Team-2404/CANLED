/*
 * CANLED ATmega328P firmware
 *
 * Authors: Conor Kelly Gerakos, Koshan Dawlaty
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

// FRC Device Type
#define DEVICE_TYPE 10

// FRC Device Manufacturer
#define DEVICE_MFG 8

// ROBORIO Disable ID
#define BROADCAST_DISABLE 0b00000000000000000000000000000

// ROBORIO CAN Heartbeat
#define HEARTBEAT 0b00001000000010001100001000000//0x01011840

// Number of LEDs used in strip -- change if different
#define NUM_LEDS 20//86
// Type of LED controller -- change if different, must be supported by FastLED - see docs: https://github.com/FastLED/FastLED/wiki/Chipset-reference
#define LED_TYPE WS2812B
// Color order for LED strip - not entirely certain what this does, as ordering is strange
#define COLOR_ORDER GRB  // GRB here but actually RGB?

bool teamColorEnableState = false;
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];
int mode = 0;
int color[] = { 0, 0, 0 };
byte dir;

struct [[gnu::packed]] RobotState {
  uint64_t matchTimeSeconds : 8;
  uint64_t matchNumber : 10;
  uint64_t replayNumber : 6;
  uint64_t redAlliance : 1;
  uint64_t enabled : 1;
  uint64_t autonomous : 1;
  uint64_t testMode : 1;
  uint64_t systemWatchdog : 1;
  uint64_t tournamentType : 3;
  uint64_t timeOfDay_yr : 6;
  uint64_t timeOfDay_month : 4;
  uint64_t timeOfDay_day : 5;
  uint64_t timeOfDay_sec : 6;
  uint64_t timeOfDay_min : 6;
  uint64_t timeOfDay_hr : 5;
};

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
  u16 color;
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
    //digitalWrite(LED_OK_PIN, HIGH);
  }

  CAN0.init_Mask(0, 1, 0x3F);  // make sure these guys are doing something -- test on prebuilt modules
  CAN0.init_Filt(0, 1, 0x37);  
  CAN0.init_Filt(1, 1, 0x37);
  CAN0.init_Filt(2, 1, 0x37);
  CAN0.init_Filt(3, 1, 0x37);
  CAN0.init_Mask(1, 1, 0b11111111111111111111111111111); // TEST
  CAN0.init_Filt(4, 1, BROADCAST_DISABLE);
  CAN0.init_Filt(5, 1, HEARTBEAT);
  // maybe incorporate other parts of the CAN packet in choosing patterns (API ID)
  CAN0.setMode(MCP_NORMAL);  // Set operation mode to normal so the MCP2515 sends acks to received data.
}

void loop() {
  //rainbow_real(&rainbow_three,15);
  //rainbowCycleSolid(&rainbow_two, 10);
  switch (mode) {
    case 1:
      {
        teamColorEnableState = false;
        fill_solid(leds, NUM_LEDS, CRGB(color[0], color[1], color[2]));
        FastLED.show();
        break;
      }
    case 2:
      {
        teamColorEnableState = false;
        rainbowCycle(&rainbow_one);
        dir = random(0, 2);
        break;
      }
    case 3:
      {
        teamColorEnableState = false;
        rainbowCycleSolid(&rainbow_two, color[0]);
        break;
      }
    case 4:
      {
        teamColorEnableState = false;
        rainbow_real(&rainbow_three, color[0]);
        break;
      }
    case 5:
      {
        teamColorEnableState = true;
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
    //digitalWrite(LED_OK_PIN, HIGH);
    CAN0.readMsgBuf(&rxId, &len, rxBuf);  // Read data: len = data length, buf = data byte(s)
    if(rxId == HEARTBEAT){
      fill_solid(leds, NUM_LEDS, CRGB(10,200,10));
      FastLED.show();
      digitalWrite(LED_OK_PIN, HIGH);
    }
    if(rxId == HEARTBEAT && teamColorEnableState){
      struct RobotState* state = (RobotState*)rxBuf;
      if((rxBuf[4] & (1 << 7)) != 0){
        fill_solid(leds, NUM_LEDS, CRGB(230,10,10));
      }else{
        fill_solid(leds, NUM_LEDS, CRGB(10,10,230));
      }
      FastLED.show();
      FastLED.delay(10);
    }
    if(rxId == BROADCAST_DISABLE && false) // TEST
    {
      while(1)
      {
        CAN0.readMsgBuf(&rxId, &len, rxBuf);
        if(rxId == HEARTBEAT && ((rxBuf[4]&&00000010)==1)){ // TEST
          loop();
        }
        delay(20);
      }; // Disables device to comply with FRC CAN spec. Will re-enable upon detecting set 'enabled' bit in CAN heartbeat.
    }

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
  struct CHSV hsv = CHSV(state->color, 255, 255);

  /* TODO: i dont know what the difference between the conversion functions are */
  //hsv2rgb_rainbow(&hsv, leds, NUM_LEDS);
  for(int i = 0; i<NUM_LEDS;i++){
    leds[i].setHSV(state->color,255,225);
    if(255-state->color<10){
      leds[i].setHSV(state->color,255,225);
    }
    
  }
  FastLED.show();
  FastLED.delay(wait);
  state->color++;
  //fill_solid(leds, NUM_LEDS, c);
  
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
    leds[i].setHSV(i * 255 / NUM_LEDS,255,255);//state->color+(2*i)
  }
  FastLED.show();
  FastLED.delay(wait);
}

