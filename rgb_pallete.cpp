#include "rgb_pallete.h"

CRGB findRidePallete(int palleteId, int position)
{
  CRGB color = findFullPallete(palleteId);
  const uint8_t hueChange = 65535 / (uint8_t)NUM_LEDS;
  
  for(int hue = 0; hue < position * 50; hue = hue + 50)
  {
    color.addToRGB(hue);
  }
  return color;
}

CRGB findFullPallete(int palleteId)
{
  if (palleteId == 1)
  {
    return CRGB::White;
  }
  else if (palleteId == 2)
  {
    return CRGB::Blue;
  }
  else if (palleteId == 3)
  {
    return CRGB::Red;
  }
  else if (palleteId == 4)
  {
    return CRGB::Yellow;
  }
  else if (palleteId == 5)
  {
    return CRGB::Green;
  }

  return CRGB::White;
}