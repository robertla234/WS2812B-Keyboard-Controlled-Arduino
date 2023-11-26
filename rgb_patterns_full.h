#ifndef RGB_PATTERNS_FULL_H
#define RGB_PATTERNS_FULL_H

// EXTERNAL LIBRARIES
#include <FastLED.h>

#include "rgb_main.h"

void rgbSetup();

// -- SYSTEM GENERAL --
void bootTestFlash(CRGB leds[]);
void clearRGB(CRGB leds[]);

// -- FULL STRIP --
void fill(CRGB leds[], CRGB crgb);
void twinkle(CRGB leds[], CRGB crgb);

// -- RIDE UP STRIP --
void tower(CRGB leds[], CRGB crgb);
void comet(CRGB leds[], CRGB crgb);

#endif
