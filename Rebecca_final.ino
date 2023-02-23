#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN        6
#define NUMPIXELS 60

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500;

unsigned long pixelPrevious = 0;        // Previous Pixel Millis
unsigned long patternPrevious = 0;      // Previous Pattern Millis
int           patternCurrent = 0;       // Current Pattern Number
int           patternInterval = 30000;   // Pattern Interval (ms)
int           pixelInterval = 50;       // Pixel Interval (ms) - time between when each pattern update is called
int           pixelQueue = 0;           // Pattern Pixel Queue
int           pixelCycle = 0;           // Pattern Pixel Cycle
uint16_t      pixelCurrent = 0;         // Pattern Current Pixel Number
int           raindropInterval = 100;     // Raindrop Pattern interval (ms)
unsigned long raindropPrevious = 0;      // Previous Raindrop Pattern Millis
int           singleFadeInterval = 10;     // Raindrop Pattern interval (ms)
unsigned long singleFadePrevious = 0;      // Previous Raindrop Pattern Millis

// LED strings
int largeInnerLeds[9] = {51, 52, 53, 54, 55, 56, 57, 58, 59}; // skip 36? 48, 49, 50, 51
int largeOuterLeds[23] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};

int smallInnerLeds[7] = {29, 30, 31, 32, 33, 34, 35};
int smallOuterLeds[17] = {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 36};

int innerLeds[18] = {27, 28, 29, 30, 31, 32, 33, 34, 35, 51, 52, 53, 54, 55, 56, 57, 58, 59};
int outerLeds[40] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};

long darkLeds[2][5] = {{2, 13, 14, 37, 45},
  { 0, 10, 22, 39, 47}
};


void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  Serial.begin(9600);
  Serial.print("testing");
  pixels.begin();
}

void colorFadeGroupOfPixels(int inner[], int loopSize, uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  uint32_t startColor = pixels.getPixelColor(inner[0]); // get the current colour
  uint8_t startR, startG, startB;
  startB = startColor & 0xFF;
  startG = (startColor >> 8) & 0xFF;
  startR = (startColor >> 16) & 0xFF;  // separate into RGB components
  while ((startR != r) || (startG != g) || (startB != b)) {
    if (startR < r) startR = startR + 2; else if (startR > r) startR = startR - 2;  // increment or decrement the old color values
    if (startG < g) startG = startG + 2; else if (startG > g) startG = startG - 2;
    if (startB < b) startB = startB + 2; else if (startB > b) startB = startB - 2;
    for (uint16_t i = 0; i < loopSize; i++) {
      pixels.setPixelColor(inner[i], startR, startG, startB);  // set the color
    }
    delay(50);
    pixels.show();
  }
  pixelQueue++;                           //  Advance current queue
  pixelCycle++;                           //  Advance current cycle
  if (pixelQueue >= 3)
    pixelQueue = 0;                       //  Loop
  if (pixelCycle >= 256)
    pixelCycle = 0;                       //  Loop
}

void colorFadeGroupOfPixelsOnce(int inner[], int loopSize, uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  for (uint16_t i = 0; i < loopSize; i++) {
    uint32_t startColor = pixels.getPixelColor(inner[i]); // get the current colour
    uint8_t startR, startG, startB;
    startB = startColor & 0xFF;
    startG = (startColor >> 8) & 0xFF;
    startR = (startColor >> 16) & 0xFF;  // separate into RGB components
    if (startR < r) startR = startR + 1; else if (startR > r) startR = startR - 1;  // increment or decrement the old color values
    if (startG < g) startG = startG + 1; else if (startG > g) startG = startG - 1;
    if (startB < b) startB = startB + 1; else if (startB > b) startB = startB - 1;
    pixels.setPixelColor(inner[i], startR, startG, startB);  // set the color
    //    Serial.println("New colors");
    //    Serial.print("R: ");
    //    Serial.print(startR);
    //    Serial.println();
    //    Serial.print("G: ");
    //    Serial.print(startG);
    //    Serial.println();
    //    Serial.print("B: ");
    //    Serial.print(startB);
    //    Serial.println();
  }
  delay(10);

  pixelQueue++;                           //  Advance current queue
  pixelCycle++;                           //  Advance current cycle
  if (pixelQueue >= 3)
    pixelQueue = 0;                       //  Loop
  if (pixelCycle >= 256)
    pixelCycle = 0;                       //  Loop
}

// Produces rain like pattern all over lights
void darkenRandomPixelsOnce() {
  // Chose random pixels to darken
  long tempDarkLeds[3] = {random(0, 40)};

  for (uint16_t i = 0; i < 1; i++) {
    uint32_t startColor = pixels.getPixelColor(outerLeds[tempDarkLeds[i]]); // get the current colour
    uint8_t startR, startG, startB;
    startB = startColor & 0xFF;
    startG = (startColor >> 8) & 0xFF;
    startR = (startColor >> 16) & 0xFF;  // separate into RGB components
    colorFadeSinglePixel(outerLeds[tempDarkLeds[i]], startR / 3, startG / 3, startB / 3, 0);
  }
}

void colorFadeSinglePixel(int pixel, uint8_t endR, uint8_t endG, uint8_t endB, int wait) {
  unsigned long currentMillis = millis();
  uint32_t startColor = pixels.getPixelColor(pixel); // get the current color
  uint8_t startR, startG, startB;
  startB = startColor & 0xFF;
  startG = (startColor >> 8) & 0xFF;
  startR = (startColor >> 16) & 0xFF;  // separate into RGB components
  while ((startR != endR) || (startG != endG) || (startB != endB)) {
    if (startR < endR) startR++; else if (startR > endR) startR--;  // increment or decrement the old color values
    if (startG < endG) startG++; else if (startG > endG) startG--;
    if (startB < endB) startB++; else if (startB > endB) startB--;

    pixels.setPixelColor(pixel, startR, startG, startB);  // set the color
    delay(5);
    pixels.show();
  }
}

// loop() function -- runs repeatedly as long as board is on ---------------
void loop() {
  unsigned long currentMillis = millis();

  if ((currentMillis - patternPrevious) >= patternInterval) { //  Check for expired time
    patternPrevious = currentMillis;
    Serial.println("Changing pattern!");
    patternCurrent++;                                         //  Advance to next pattern

    if (patternCurrent >= 28)
      patternCurrent = 0;
  }
  pixelPrevious = currentMillis;                            //  Run current frame
  switch (patternCurrent) {
    case 27:
      Serial.println("IN CASE 27");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 20, 0, 255, 0); // Purple
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 166, 54, 255, 0); // Pastel Purple

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7,  0, 0, 255, 0); // Blue
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 20, 112, 0); // Hot Pink
      break;
    case 26:
      Serial.println("IN CASE 26");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 166, 54, 255, 0); // Pastel Purple
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 0, 0, 255, 0); // Blue

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 20, 112, 0); // Hot Pink
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 165, 0, 0); // Orange
      break;
    case 25:
      Serial.println("IN CASE 25");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 0, 255, 0); // Blue
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 0, 218, 255, 0); // Light Blue

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 165, 0, 0); // Orange
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 30, 0, 0); // Hot Red
      break;
    case 24:
      Serial.println("IN CASE 24");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 113, 255, 0); // Light Blue
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 2, 255, 93, 0); // Green

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 30, 0, 0); // Hot Red
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 77, 0, 255, 0); // Purple
      break;
    case 23:
      Serial.println("IN CASE 23");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 2, 255, 93, 0); // Green
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 74, 255, 72, 0); // Pastel Green

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 77, 0, 255, 0); // Purple
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 69, 108, 0); // Pastel Red
      break;
    case 22:
      Serial.println("IN CASE 22");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 74, 255, 72, 0); // Pastel Green
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 255, 255, 70, 0); // Pastel Yellow

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 69, 108, 0); // Pastel Red
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 134, 71, 0); // Pastel Salmon
      break;
    case 21:
      Serial.println("IN CASE 21");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 255, 255, 70, 0); // Pastel Yellow
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 100, 100, 255, 0); // Pastel Blue

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 134, 71, 0); // Pastel Salmon
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 100, 100, 0); // Pastel Pink
      break;
    case 20:
      Serial.println("IN CASE 20");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 100, 100, 255, 0); // Pastel Blue
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 0, 0, 0, 0); // Black

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 100, 100, 0); // Pastel Pink
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 0, 0, 0, 0); // Black
      break;
    case 19:
      Serial.println("IN CASE 19");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 0, 0, 0); // Black
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 0, 0, 0, 0); // Black

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 0, 0, 0, 0); // Black
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 0, 0, 0, 0); // Black
      break;
    case 18:
      Serial.println("IN CASE 18");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 0, 0, 0); // Black
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 238, 255, 232, 0); // Cool White

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 0, 0, 0, 0); // Black
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 238, 255, 232, 0); // Cool White
      break;
    case 17:
      Serial.println("IN CASE 17");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 238, 255, 232, 0); // Cool White
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 238, 255, 232, 0); // Cool White

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 238, 255, 232, 0); // Cool White
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 238, 255, 232, 0); // Cool White
      break;
    case 16:
      Serial.println("IN CASE 16");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 238, 255, 232, 0); // Cool White
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 0, 255, 0, 0); // Green

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 238, 255, 232, 0); // Cool White
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 0, 255, 0, 0); // Green
      break;
    case 15:
      Serial.println("IN CASE 15");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 255, 0, 0); // Green
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 0, 255, 0, 0); // Green

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 0, 255, 0, 0); // Green
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 0, 255, 0, 0); // Green
      break;
    case 14:
      Serial.println("IN CASE 14");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 255, 0, 0); // Green
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 0, 255, 0, 0); // Green

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 0, 255, 0, 0); // Green
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 0, 255, 0, 0); // Green
      break;
    case 13:
      Serial.println("IN CASE 13");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 255, 0, 0); // Green
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 0, 0, 255, 0); // Blue

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 0, 255, 0, 0); // Green
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 0, 0, 255, 0); // Blue
      break;
    case 12:
      Serial.println("IN CASE 12");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 255, 0, 0); // Green
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23,0, 0, 255, 0); // Blue

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 0, 255, 0, 0); // Green
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 0, 0, 255, 0); // Blue
      break;
    case 11:
      Serial.println("IN CASE 11");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 0, 255, 0); // Blue
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 0, 0, 255, 0); // Blue

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 0, 0, 255, 0); // Blue
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 0, 0, 255, 0); // Blue
      break;
    case 10:
      Serial.println("IN CASE 10");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 0, 255, 0); // Blue
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 0, 0, 255, 0); // Blue

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 0, 0, 255, 0); // Blue
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 0, 0, 255, 0); // Blue
      break;
    case 9:
      Serial.println("IN CASE 9");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 0, 255, 0); // Blue
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 255, 0, 0, 0); // Red

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 0, 0, 255, 0); // Blue
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 0, 0, 0); // Red
      break;
    case 8:
      Serial.println("IN CASE 8");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 0, 0, 255, 0); // Blue
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 255, 0, 0, 0); // Red

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 0, 11, 255, 0); // Blue
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 0, 0, 0); // Red
      break;
    case 7:
      Serial.println("IN CASE 7");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 255, 0, 0, 0); // Red
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 255, 0, 0, 0); // Red

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 0, 0, 0); // Red
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 0, 0, 0); // Red
      break;
    case 6:
      Serial.println("IN CASE 6");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 255, 0, 0, 0); // Red
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 255, 0, 0, 0); // Red

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 0, 0, 0); // Red
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 0, 0, 0); // Red
      break;
    case 5:
      Serial.println("IN CASE 5");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 255, 0, 0, 0); // Red
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 255, 237, 192, 0); // Warm White

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 0, 0, 0); // Red
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 237, 192, 0); // Warm White
      break;
    case 4: // inner loops taking forever to change here??
      Serial.println("IN CASE 4");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 255, 0, 0, 0); // Red
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 255, 237, 192, 0); // Warm White

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 0, 0, 0); // Red
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 237, 192, 0); // Warm White
      break;
    case 3:
      Serial.println("IN CASE 3");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 255, 237, 192, 0); // Warm White
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 255, 237, 192, 0); // Warm White

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 237, 192, 0); // Warm White
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 237, 192, 0); // Warm White
      break;
    case 2:
      Serial.println("IN CASE 2");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 255, 237, 192, 0); // Warm White
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 20, 0, 255, 0); // Periwinkle

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 237, 192, 0); // Warm White
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 60, 0, 0); // Orange
      break;
    case 1:
      Serial.println("IN CASE 1");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 20, 0, 255, 0); // Periwinkle
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 255, 0, 165, 0); // Hot Pink

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 60, 0, 0); // Orange
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 255, 0, 0, 0); // Red
      break;
    case 0:
      Serial.println("IN CASE 0");
      colorFadeGroupOfPixelsOnce(largeInnerLeds, 9, 255, 0, 165, 0); // Hot Pink
      colorFadeGroupOfPixelsOnce(largeOuterLeds, 23, 20, 0, 255, 0); // Purple

      colorFadeGroupOfPixelsOnce(smallInnerLeds, 7, 255, 0, 0, 0); // Red
      colorFadeGroupOfPixelsOnce(smallOuterLeds, 17, 0, 0, 255, 0); // Blue
      break;
    default: // Execute raindrop if case not broken
      if (currentMillis - raindropPrevious >= raindropInterval) {
        raindropPrevious = currentMillis;
        darkenRandomPixelsOnce();
      }
  }
  if (currentMillis - raindropPrevious >= raindropInterval) {
    raindropPrevious = currentMillis;
    darkenRandomPixelsOnce();
  }

  pixels.show();
}
