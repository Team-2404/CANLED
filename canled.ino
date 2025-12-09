// MODIFIED FROM CAN Receive Example
//
// todo: use hsv
#include <mcp_can.h>
#include <SPI.h>
#include <FastLED.h>  // maybe a better lib than fastled? something lighter

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];
#define DATA_PIN 3
#define DEVICE_ID 55  // FRC device ID - The one you'd set for motors etc.
#define NUM_LEDS 86
#define COLOR_ORDER GRB  // for some reason, setting this to GRB means fill_solid() actually uses RGB and vice versa.
#define LED_TYPE WS2812B
#define CAN0_INT 2  // Set INT to pin 2
MCP_CAN CAN0(10);   // Set CS to pin 10
CRGB leds[NUM_LEDS];
int mode = 0;
int color[] = { 0, 0, 0 };
byte dir;

struct rainbow_state {
  u8 index;
  const u16 wait;
  const u8 dim;
};

struct solid_rainbow_state {
  // const u16 wait;
  // const u8 dim;
  // u8 color[3];
  // /* these dont go above 2. maybe use a bitfield? */
  // u8 increase;
  // u8 decrease;
  // u8 num_color;
  u16 wait;
  u8 color;
};

#define SOLID_RAINBOW_INIT(_wait) (struct solid_rainbow_state) { .wait = _wait,.color = 0}
#define RAINBOW_INIT(_wait, _dim) \
  (struct rainbow_state) { \
    .index = 0, .wait = _wait, .dim = _dim \
  }

struct rainbow_state rainbow_one = RAINBOW_INIT(10, 6);
struct solid_rainbow_state rainbow_two = SOLID_RAINBOW_INIT(20);

void setup() {
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  if (CAN0.begin(MCP_EXT, CAN_1000KBPS, MCP_8MHZ) == CAN_OK) {
    digitalWrite(4, HIGH);
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
        rainbowCycleSolid(&rainbow_two);
        break;
      }
    default:
      {
        break;
      }
  }
  digitalWrite(5, LOW);
  //digitalWrite(4, LOW);

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

void rainbowCycle(struct rainbow_state* state) {
  //loop through all colors in the wheel
  state->index--;
  if (state->index < 0) {
    state->index = 255;
  }

  //Set RGB color of each LED
  for (int i = 0; i < NUM_LEDS; i++) {
    CRGB ledColor = wheel(((i * 256 / NUM_LEDS) + state->index) % 256, state->dim);
    leds[i] = ledColor;
  }

  FastLED.show();
  FastLED.delay(state->wait);
}


CRGB wheel(int WheelPos, int dim) {
  CRGB color;  // orgininally R G B ordering
  if (85 > WheelPos) {
    color.b = 0;
    color.g = WheelPos * 3 / dim;
    color.r = (255 - WheelPos * 3) / dim;
  } else if (170 > WheelPos) {
    color.b = WheelPos * 3 / dim;
    color.g = (255 - WheelPos * 3) / dim;
    color.r = 0;
  } else {
    color.b = (255 - WheelPos * 3) / dim;
    color.g = 0;
    color.r = WheelPos * 3 / dim;  // *3
  }
  return color;
}


void rainbowCycleSolid(struct solid_rainbow_state* state) {
  /* TODO: i dont know what the diffrence between the conversion functions are */
  CRGB c = hsv2rgb_rainbow(CHSV(state->color, 255, 255));
  state->color++;
  fill_solid(leds, NUM_LEDS, c);\
  FastLED.show();
  FastLED.delay(state->wait);
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

/*********************************************************************************************************
  END FILE
*******************************************************************************************/