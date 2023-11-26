#ifndef RGB_PALLETE_H
#define RGB_PALLETE_H

// EXTERNAL LIBRARIES
#include <FastLED.h>

#include "rgb_main.h"

CRGB findRidePallete(int palleteId, int position);
CRGB findFullPallete(int palleteId);

#endif