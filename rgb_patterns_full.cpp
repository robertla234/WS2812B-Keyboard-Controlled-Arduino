#include "rgb_patterns_full.h"

// -- SYSTEM GENERAL --
void bootTestFlash(CRGB leds[])
{
  leds[0] = CRGB::Red;
  leds[1] = CRGB::Red;
  leds[2] = CRGB::Red;
  leds[NUM_LEDS - 1] = CRGB::Red;
  leds[NUM_LEDS - 2] = CRGB::Red;
  leds[NUM_LEDS - 3] = CRGB::Red;
  FastLED.show();
  delay(400);
  FastLED.clear(true);
  delay(200);

  leds[0] = CRGB::Green;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Green;
  leds[NUM_LEDS - 1] = CRGB::Green;
  leds[NUM_LEDS - 2] = CRGB::Green;
  leds[NUM_LEDS - 3] = CRGB::Green;
  FastLED.show();
  delay(400);
  FastLED.clear(true);
  delay(200);

  leds[0] = CRGB::Blue;
  leds[1] = CRGB::Blue;
  leds[2] = CRGB::Blue;
  leds[NUM_LEDS - 1] = CRGB::Blue;
  leds[NUM_LEDS - 2] = CRGB::Blue;
  leds[NUM_LEDS - 3] = CRGB::Blue;
  FastLED.show();
  delay(400);
  FastLED.clear(true);
  delay(200);
}

void clearRGB(CRGB leds[])
{
  fill(leds, CRGB::Black);
}

// -- FULL STRIP --
void fill(CRGB leds[], CRGB crgb)
{
  fill_solid(leds, NUM_LEDS, crgb);
  FastLED.show();
}

void twinkle(CRGB leds[], CRGB crgb)
{ 
  for (int i = 0; i < 8; i++)
  {
    leds[random(NUM_LEDS - 2)] = CRGB::Black;
    leds[random(NUM_LEDS - 2)] = crgb;
  }

  crgb.fadeToBlackBy(100);

  for (int j = 0; j < 8; j++)
  {
    leds[random(NUM_LEDS - 2)] = crgb;
  }

  crgb = blend(crgb, CRGB::White, NUM_LEDS/2);

  for (int k = 0; k < 8; k++)
  {
    leds[random(NUM_LEDS - 2)] = crgb;
  }
}

// -- RIDE UP STRIP --
void tower(CRGB leds[], CRGB crgb)
{
  leds[7] = crgb;
  leds[6] = crgb.fadeToBlackBy(215);
  leds[5] = crgb;
  leds[4] = crgb;
  leds[3] = crgb;
  leds[2] = crgb;
  leds[1] = crgb;
  leds[0] = crgb;
}

void comet(CRGB leds[], CRGB crgb)
{
  leds[7] = crgb;
  leds[6] = crgb.fadeToBlackBy(215);
  leds[5] = crgb;
  leds[4] = crgb;
  leds[3] = crgb.fadeToBlackBy(150);
  leds[2] = crgb;
  leds[1] = crgb;
  leds[0] = CRGB(0,0,0);
}
