#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define DEBUG false
#define BTN_NEXT_PIN 2
#define BTN_FIX_PIN 3
#define LEDS_PIN 6       // What output pin is the LED strip data line?
#define NUM_LEDS 150     // How many LEDs are there in the strip?
#define FRAME_DELAY 75   // Time between frames in ms (smaller is faster)
#define SCAN_FRAMES 500  // How many frames to stay on a single mode
#define MAX_MODE 16      // Number of the last mode

#define EEPROM_ADDRESS 0  // address in EEPROM to save selected mode

#define COLOR_WHITE strip.Color(255, 255, 255)
#define COLOR_BLACK strip.Color(0, 0, 0)

#define COLORS_LENGTH 3

uint8_t mode;    // What display are we doing?
uint16_t frame;  // How long have we been doing it?
uint8_t selected = -1;
uint32_t buffer[50];

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LEDS_PIN, NEO_RGB + NEO_KHZ800);

const uint8_t colors[][3] = {
    {255, 0, 0}, 
    {0, 255, 0}, 
    {0, 0, 255},
};

void setup() {
  pinMode(BTN_NEXT_PIN, INPUT);
  pinMode(BTN_FIX_PIN, INPUT);

  randomSeed(analogRead(0));

  Serial.begin(9600);

  strip.begin();
  strip.setBrightness(64);
  //strip.show();

  mode = EEPROM.read(EEPROM_ADDRESS);
  if (mode > MAX_MODE) {
    mode = 0;
  }
  frame = 0;
  writelog("===================\ninitial mode=", mode);
}

void loop() {
  switch (mode) {
    case 0:
      if (frame == 0) {
        selected++;
      }
      strip.setPixelColor(frame, color(selected));
      if (frame > NUM_LEDS) {
        if (selected == COLORS_LENGTH - 1) {
          frame = SCAN_FRAMES;
        } else {
          frame = -1;
        }
      }
      break;
    case 1:
      rainbow(25);
      break;
    case 2:  // Candy stripes (white-red)
      if (frame == 0) {
        fillBufferSolid(0, 10, COLOR_WHITE);
        fillBufferSolid(10, 10, color(0));
        fillBufferSolid(20, 10, COLOR_BLACK);
      }
      drawMarquee(30);
      break;
    case 3:  // Candy stripes (red-green)
      if (frame == 0) {
        fillBufferSolid(0, 10, COLOR_WHITE);
        fillBufferSolid(10, 10, color(1));
        fillBufferSolid(20, 10, COLOR_BLACK);
      }
      drawMarquee(30);
      break;
    case 4:  // Candy stripes (green-blue)
      if (frame == 0) {
        fillBufferSolid(0, 10, COLOR_WHITE);
        fillBufferSolid(10, 10, color(2));
        fillBufferSolid(20, 10, COLOR_BLACK);
      }
      drawMarquee(30);
      break;
    case 5:  // Thin green stripes
      if (frame == 0) {
        fillBufferSolid(0, 3, color(0));
        fillBufferSolid(3, 7, COLOR_BLACK);
      }
      drawMarquee(10);
      break;
    case 6:  // Thin red stripes
      if (frame == 0) {
        fillBufferSolid(0, 3, color(1));
        fillBufferSolid(3, 7, COLOR_BLACK);
      }
      drawMarquee(10);
      break;
    case 7:  // Thin blue stripes
      if (frame == 0) {
        fillBufferSolid(0, 3, color(2));
        fillBufferSolid(3, 7, COLOR_BLACK);
      }
      drawMarquee(10);
      break;
    case 8:  // Red/black gradient stripes
      if (frame == 0) {
        fillBufferGradient(0, 10, 0, 0, 0, colors[0][0], colors[0][1], colors[0][2]);
        fillBufferGradient(10, 10, colors[0][0], colors[0][1], colors[0][2], 0, 0, 0);
      }
      drawMarquee(20);
      break;
    case 9:  // Green/black gradient stripes
      if (frame == 0) {
        fillBufferGradient(0, 10, 0, 0, 0, colors[1][0], colors[1][1], colors[1][2]);
        fillBufferGradient(10, 10, colors[1][0], colors[1][1], colors[1][2], 0, 0, 0);
      }
      drawMarquee(20);
      break;
    case 10:  // Blue/black gradient stripes
      if (frame == 0) {
        fillBufferGradient(0, 10, 0, 0, 0, colors[2][0], colors[2][1],
                           colors[2][2]);
        fillBufferGradient(10, 10, colors[2][0], colors[2][1], colors[2][2], 0, 0, 0);
      }
      drawMarquee(20);
      break;
    case 11:  // White/black gradient stripes
      if (frame == 0) {
        fillBufferGradient(0, 10, 0, 0, 0, 255, 255, 255);
        fillBufferGradient(10, 10, 255, 255, 255, 0, 0, 0);
      }
      drawMarquee(20);
      break;
    case 12:
      if (frame == 0) {
        selected = random(COLORS_LENGTH);
      }
      drawSolid(strip.Color(0, 0, 0));
      colorSnake(1 + random(4), color(selected), FRAME_DELAY + 10 * random(7), false);
      break;
    case 13:
      if (frame == 0) {
        fillBufferSolid(0, 5, strip.Color(255, 0, 0));
        fillBufferSolid(5, 5, strip.Color(255, 255, 0));
        fillBufferSolid(10, 5, strip.Color(0, 255, 0));
        fillBufferSolid(15, 5, strip.Color(0, 255, 255));
        fillBufferSolid(20, 5, strip.Color(0, 0, 255));
        fillBufferSolid(25, 5, strip.Color(255, 0, 255));
      }
      drawMarquee(30);
      break;
    case 14:
      RandomColor();
      break;
    case 15:
      RandomWhite();
      break;
    case 16:
      uint16_t frame2 = (frame * 10) % 511;
      if (frame2 > 255) {
        frame2 = 511 - frame2;
      }
      drawSolid(strip.Color(frame2, frame2, frame2));
  }

  strip.show();
  frame++;
  delay(FRAME_DELAY);

  bool next = false;
  if (digitalRead(BTN_NEXT_PIN) == HIGH) {
    if (frame == 1) {
      delay(500);
    } else {
      delay(25);
    }
    if (digitalRead(BTN_NEXT_PIN) == HIGH) {
      next = true;
    }
  } else if (frame > SCAN_FRAMES) {
    if (digitalRead(BTN_FIX_PIN) == LOW) {
      delay(25);
      if (digitalRead(BTN_FIX_PIN) == LOW) {
        next = true;
      }
    }
    if (!next) {
    writelog("===================\nfixed mode=", mode);
      frame = 0;
      selected = -1;
    }
  }
  if (next) {
    if (mode < MAX_MODE) {
      mode++;
    } else {
      mode = 0;
    }
    EEPROM.write(EEPROM_ADDRESS, mode);
    frame = 0;
    selected = -1;
    writelog("===================\nnew mode=", mode);
  }
  writelog("frame=", frame);
}

void colorSnake(uint8_t l, uint32_t c, uint8_t wait, boolean reverse) {
  uint32_t no = strip.Color(0, 0, 0);
  uint16_t s = strip.numPixels();

  int16_t from = reverse ? s + l + 1 : l;
  int16_t to = reverse ? 1 : s + 2 * l;
  int16_t diff = reverse ? -1 : 1;

  int16_t i = from + diff * frame;
  int16_t jon = i - l - 1;
  int16_t joff = reverse ? i - 1 : i - 2 * l - 1;
  //Serial.print(i);
  //Serial.print("->");
  //Serial.print(jon);
  //Serial.print(",");
  //Serial.println(joff);
  if (jon >= 0 && jon < s) {
    strip.setPixelColor(jon, c);
  }
  if (joff >= 0 && joff < s) {
    strip.setPixelColor(joff, no);
  }
  if (!(from <= i && i <= to) && !(from >= i && i >= to)) {
    frame = SCAN_FRAMES;
  }
}

void rainbow(uint8_t wait) {
  uint16_t i;
  for (i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, Wheel((i + frame) & 255));
  }
}

void fillBufferSolid(uint8_t start, uint8_t size, uint32_t color) {
  uint8_t i;
  for (i = start; i != (start + size); i++) {
    buffer[i] = color;
  }
}

void fillBufferGradient(uint8_t start, uint8_t size, uint8_t r0, uint8_t g0,
                        uint8_t b0, uint8_t r1, uint8_t g1, uint8_t b1) {
  uint8_t i;
  float sizeF = size;
  for (i = 0; i != size; i++) {
    buffer[i + start] =
        strip.Color((r0 * ((size - i) / sizeF)) + (r1 * (i / sizeF)),
                    (g0 * ((size - i) / sizeF)) + (g1 * (i / sizeF)),
                    (b0 * ((size - i) / sizeF)) + (b1 * (i / sizeF)));
  }
}

// Create sets of random white or gray pixels
void RandomWhite() {
  int V, j;
  for (j = 0; j < strip.numPixels(); j++) {
    V = random(255);
    strip.setPixelColor(j, V, V, V);
  }
  delay(100);
}

// Create sets of random colors
void RandomColor() {
  int j;
  for (j = 0; j < strip.numPixels(); j++) {
    strip.setPixelColor(j, random(255), random(255), random(255));
  }
  delay(100);
};

void drawMarquee(uint8_t size) {
  uint8_t i;
  for (i = 0; i != NUM_LEDS; i++) {
    strip.setPixelColor(i, buffer[(i + frame) % size]);
  }
}

void drawSolid(uint32_t color) {
  uint8_t i;
  for (i = 0; i != NUM_LEDS; i++) {
    strip.setPixelColor(i, color);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

uint32_t color(uint8_t num) {
  return strip.Color(colors[num][0], colors[num][1], colors[num][2]);
}

void writelog(char *text) {
  if (DEBUG) {
    Serial.println(text);
  }
}

template <class T> void writelog(String text, T value) {
  if (DEBUG) {
    Serial.print(text);
    Serial.println(value);
  }
}
