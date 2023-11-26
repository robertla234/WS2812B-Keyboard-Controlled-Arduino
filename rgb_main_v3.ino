// ----------------------------------------------------
// BIOS Mode K57 - (While Plugged In) Turn off, hold B + S, Turn On
// ----------------------------------------------------


// ----------------------------------------------------
// INTERNAL REFERENCES
// ----------------------------------------------------
#include "rgb_main.h"
#include "rgb_patterns_full.h"
#include "rgb_audio_mode.h"
#include "rgb_pallete.h"
#include <hidboot.h>
#include <usbhub.h>
// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

// ----------------------------------------------------
// LED Variables
// ----------------------------------------------------
CRGB ledsL[NUM_LEDS];
CRGB ledsR[NUM_LEDS];

CRGB RideLedsL[NUM_LEDS];
CRGB RideLedsR[NUM_LEDS];
CRGB hideRideLedsL[9];
CRGB hideRideLedsR[9];
CRGB hideTemp[9];

CRGB FullLedsL[NUM_LEDS];
CRGB FullLedsR[NUM_LEDS];
CRGB hideFullLeds[NUM_LEDS];
bool isFullInitial = true;
int FullLedMode = 0; // 0 - Fill and Fade, 1 - Always On, 2 - Repeaters, 3 - Strobe

CRGB strobeColor = CRGB::White;

int halfway = NUM_LEDS / 2;
bool testMode = true;
bool mode = true;       // 0 - Audio Reactive Mode (DEFAULT), 1 - Controller Mode
bool stallBool = false;
int controllerMode = 0; // 0 - Comet (DEFAULT), 1 - Tower
int fullPalleteMode = 0; // see rgb_pallete
int ridePalleteMode = 0;

int fullHalfMode = 0;
int strobeMode = 0;
int twinkleStall = 0;

// ----------------------------------------------------
// Keyboard USB Host Shield Setup
// ----------------------------------------------------
class KbdRptParser : public KeyboardReportParser
{
protected:
  void OnControlKeysChanged(uint8_t before, uint8_t after);
  void OnKeyDown(uint8_t mod, uint8_t key);
  void OnKeyPressed(uint8_t key);
};
void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  uint8_t c = OemToAscii(mod, key);
  if (c)
  {
    OnKeyPressed(c);
  }
}
void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after)
{
  MODIFIERKEYS beforeMod;
  *((uint8_t *)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t *)&afterMod) = after;

  if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl)
  {
    Serial.println("LeftCtrl changed");
  }
  if (beforeMod.bmLeftShift != afterMod.bmLeftShift)
  {
    Serial.println("LeftShift changed");
  }
  if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt)
  {
    Serial.println("LeftAlt changed");
  }
  if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI)
  {
    Serial.println("LeftGUI changed");
  }

  if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl)
  {
    Serial.println("RightCtrl changed");
  }
  if (beforeMod.bmRightShift != afterMod.bmRightShift)
  {
    Serial.println("RightShift changed");
  }
  if (beforeMod.bmRightAlt != afterMod.bmRightAlt)
  {
    Serial.println("RightAlt changed");
  }
  if (beforeMod.bmRightGUI != afterMod.bmRightGUI)
  {
    Serial.println("RightGUI changed");
  }
}
void KbdRptParser::OnKeyPressed(uint8_t key)
{
  Serial.print("ASCII: ");
  Serial.println((char)key);

  // LED-to-Key Assignments
  //  -- MODE SWITCHING
  if ((char)key == '[')
  {
    controllerMode = 0;
    Serial.print(" -- CONTROLLER MODE on 0 Comet -- ");
  }
  if ((char)key == ']')
  {
    controllerMode = 1;
    Serial.print(" -- CONTROLLER MODE on 1 Tower -- ");
  }

  CRGB selectedColor = CRGB(255, 255, 255);
  CRGB *selectedSide = hideTemp;

  #pragma region LEFT/RIGHT Ride Controls
  // -- LEFT --
  if ((char)key == 'u')
  {
    selectedSide = hideRideLedsL;
    selectedColor = findRidePallete(ridePalleteMode, 0);
  }
  if ((char)key == 'i')
  {
    selectedSide = hideRideLedsL;
    selectedColor = findRidePallete(ridePalleteMode, 1);
  }
  if ((char)key == 'j')
  {
    selectedSide = hideRideLedsL;
    selectedColor = findRidePallete(ridePalleteMode, 2);
  }
  if ((char)key == 'k')
  {
    selectedSide = hideRideLedsL;
    selectedColor = findRidePallete(ridePalleteMode, 3);
  }
  if ((char)key == 'm')
  {
    selectedSide = hideRideLedsL;
    selectedColor = findRidePallete(ridePalleteMode, 4);
  }
  if ((char)key == ',')
  {
    selectedSide = hideRideLedsL;
    selectedColor = findRidePallete(ridePalleteMode, 5);
  }

  // -- RIGHT --
  if ((char)key == 'p')
  {
    selectedSide = hideRideLedsR;
    selectedColor = findRidePallete(ridePalleteMode, 0);
  }
  if ((char)key == 'o')
  {
    selectedSide = hideRideLedsR;
    selectedColor = findRidePallete(ridePalleteMode, 1);
  }
  if ((char)key == ';')
  {
    selectedSide = hideRideLedsR;
    selectedColor = findRidePallete(ridePalleteMode, 2);
  }
  if ((char)key == 'l')
  {
    selectedSide = hideRideLedsR;
    selectedColor = findRidePallete(ridePalleteMode, 3);
  }
  if ((char)key == '/')
  {
    selectedSide = hideRideLedsR;
    selectedColor = findRidePallete(ridePalleteMode, 4);
  }
  if ((char)key == '.')
  {
    selectedSide = hideRideLedsR;
    selectedColor = findRidePallete(ridePalleteMode, 5);
  }

  // -- FINAL SENDER --
  if (controllerMode == 0)
  {
    comet(selectedSide, selectedColor);
  }
  else if (controllerMode == 1)
  {
    tower(selectedSide, selectedColor);
  }
  #pragma endregion LEFT/RIGHT Ride Controls

  #pragma region Full Controls
  // --FILL AND FADE--
  if ((char)key == 'q')
  {
    fill(hideFullLeds, findFullPallete(fullPalleteMode));
    FullLedMode = 0;
    isFullInitial = true;
  }
  if ((char)key == 'w')
  {
    fill(hideFullLeds, CRGB::White);
    FullLedMode = 0;
    isFullInitial = true;
  }

  // --MIDDLE FILLS--
  if ((char)key == 'e')
  {
    clearRGB(hideFullLeds);
    hideFullLeds[halfway] = findFullPallete(fullPalleteMode); // ETERENAL White
    hideFullLeds[halfway + 1] = findFullPallete(fullPalleteMode);
    stallBool = false;
    FullLedMode = 2;
    isFullInitial = true;
  }
  if ((char)key == 'r')
  {
    clearRGB(hideFullLeds);
    hideFullLeds[halfway] = CRGB::White; // ETERENAL White
    hideFullLeds[halfway + 1] = CRGB::White;
    stallBool = false;
    FullLedMode = 2;
    isFullInitial = true;
  }

  // --ALWAYS ON--
  if ((char)key == 'a')
  {
    fill(hideFullLeds, findFullPallete(fullPalleteMode));
    fullHalfMode = 1;
    FullLedMode = 1;
    isFullInitial = true;
  }
  if ((char)key == 's')
  {
    fill(hideFullLeds, findFullPallete(fullPalleteMode));
    fullHalfMode = 2;
    FullLedMode = 1;
    isFullInitial = true;
  }
  if ((char)key == 'd')
  {
    fill(hideFullLeds, findFullPallete(fullPalleteMode));
    fullHalfMode = 0;
    FullLedMode = 1;
    isFullInitial = true;
  }
  if ((char)key == 'f')
  {
    fill(hideFullLeds, CRGB::White); // ETERENAL White
    fullHalfMode = 0;
    FullLedMode = 1;
    isFullInitial = true;
  }

  // --REPEATERS--
  if ((char)key == 'z')
  {
    twinkle(hideFullLeds, findFullPallete(fullPalleteMode));
    twinkleStall = 0;
    FullLedMode = 3;
    isFullInitial = true;
  }
  if ((char)key == 'x')
  {
    //twinkle(hideFullLeds, findFullPallete(fullPalleteMode));
    FullLedMode = 9;
    isFullInitial = true;
  }
  if ((char)key == 'c')
  {
    //twinkle(hideFullLeds, findFullPallete(fullPalleteMode));
    FullLedMode = 9;
    isFullInitial = true;
  }
  if ((char)key == 'v')
  {
    //twinkle(hideFullLeds, findFullPallete(fullPalleteMode));
    FullLedMode = 9;
    isFullInitial = true;
  }

  // --STROBE--
  if ((char)key == 'b')
  {
    strobeColor = findFullPallete(fullPalleteMode);
    if (strobeMode > 0)
    {
      strobeMode = 0;
    }
    else
    {
      strobeMode = 1;
    }

    FullLedMode = 10; //--INCREMENT UP IF NEEDED
    isFullInitial = false;
  } 

  // --CLEAR--
  if ((char)key == ' ')
  {
    clearRGB(hideFullLeds);
    FullLedMode = 10; //--INCREMENT UP IF NEEDED
    isFullInitial = true;
  }
  #pragma endregion

  #pragma region Pallete Controls
  if ((char)key == '1')
  {
    fullPalleteMode = 1;
  }
  if ((char)key == '2')
  {
    fullPalleteMode = 2;
  }
  if ((char)key == '3')
  {
    fullPalleteMode = 3;
  }
  if ((char)key == '4')
  {
    fullPalleteMode = 4;
  }
  if ((char)key == '5')
  {
    fullPalleteMode = 5;
  }

  if ((char)key == '6')
  {
    ridePalleteMode = 1;
  }
  if ((char)key == '7')
  {
    ridePalleteMode = 2;
  }
  if ((char)key == '8')
  {
    ridePalleteMode = 3;
  }
  if ((char)key == '9')
  {
    ridePalleteMode = 4;
  }
  if ((char)key == '0')
  {
    ridePalleteMode = 5;
  }
  #pragma endregion
};

// ----------------------------------------------------
// Keyboard USB Host Shield Variables
// ----------------------------------------------------
USB Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);
KbdRptParser Prs;

// ----------------------------------------------------
// Arduino Required Setup
// ----------------------------------------------------
void setup()
{
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial)
    ; // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Start");
  if (Usb.Init() == -1)
  {
    Serial.println("OSC did not start.");
  }
  delay(200);

  HidKeyboard.SetReportParser(0, &Prs);
  FastLED.addLeds<WS2812B, 2, GRB>(ledsL, NUM_LEDS);
  FastLED.addLeds<WS2812B, 4, GRB>(ledsR, NUM_LEDS);
  FastLED.setBrightness(130);
}

void loop()
{
  // CONNECTION TEST
  if (testMode)
  {
    bootTestFlash(ledsL);
    bootTestFlash(ledsR);
    testMode = false;
  }

  Usb.Task();

  if (mode) // TRIGGERABLE MODE
  {
    // Ride LED Logic
    for (int j = 8; j > 0; j--)
    {
      hideRideLedsL[j] = hideRideLedsL[j - 1];
      hideRideLedsR[j] = hideRideLedsR[j - 1];
    }

    for (int i = NUM_LEDS - 1; i > 0; i--)
    {
      RideLedsL[i] = RideLedsL[i - 1];
      RideLedsR[i] = RideLedsR[i - 1];
    }

    RideLedsL[0] = hideRideLedsL[8];
    RideLedsR[0] = hideRideLedsR[8];

    // Full LED Logic
    for (int k = 0; k < NUM_LEDS; k++)
    {
      if (isFullInitial)
      {
        FullLedsL[k] = hideFullLeds[k];
        FullLedsR[k] = hideFullLeds[k];

        if (FullLedMode == 1)
        {        
          if (fullHalfMode == 1 && k < NUM_LEDS/2)
          {
            FullLedsL[k] = CRGB::Black;
            FullLedsR[k] = CRGB::Black;
          }
          else if (fullHalfMode == 2 && k >= NUM_LEDS/2)
          {
            FullLedsL[k] = CRGB::Black;
            FullLedsR[k] = CRGB::Black;
          }
        }
      }
      else if (FullLedMode == 0) // FILL AND FADE
      {
        FullLedsL[k].fadeToBlackBy(4);
        FullLedsR[k].fadeToBlackBy(4);
      }
      else if (FullLedMode == 1) // ALWAYS ON
      {
        if (fullHalfMode == 1 && k < NUM_LEDS/2)
        {
          FullLedsL[k] = CRGB::Black;
          FullLedsR[k] = CRGB::Black;
        }
        else if (fullHalfMode == 2 && k >= NUM_LEDS/2)
        {
          FullLedsL[k] = CRGB::Black;
          FullLedsR[k] = CRGB::Black;
        }
      }
      else if (FullLedMode == 2) // MIDDLE FILLS
      {
        if (k < halfway && !stallBool)
        {
          FullLedsL[k] = FullLedsL[k + 1];
          FullLedsR[k] = FullLedsR[k + 1];
        }
        else if (k >= halfway && !stallBool)
        {
          FullLedsL[k] = FullLedsL[NUM_LEDS - k - 1];
          FullLedsR[k] = FullLedsR[NUM_LEDS - k - 1];
        }
      }
      else if (FullLedMode == 3)
      {
        FullLedsL[k] = hideFullLeds[k];
        FullLedsR[k] = hideFullLeds[k];
      }
      else if (strobeMode > 0) // STROBE
      {
        if (strobeMode == 1)
        {
          FullLedsL[k] = strobeColor;
          FullLedsR[k] = strobeColor;
        }
        else 
        {
          FullLedsL[k] = CRGB::Black;
          FullLedsR[k] = CRGB::Black;
        }
      }
      else{
        // FUTURE IMPLEMENTATION
          FullLedsL[k] = CRGB::Black;
          FullLedsR[k] = CRGB::Black;
      }
    }

    // Bool Resets
    if (isFullInitial)
    {
      isFullInitial = false;
    }
    if (stallBool)
    {
      stallBool = false;
    }

    // Strobe Mode Logic
    if (strobeMode == 1)
    {
      strobeMode = 2;
    }
    else if (strobeMode == 2)
    {
      strobeMode = 1;
    }

    // FullLEDMode Repeater Reset
    if (FullLedMode == 3)
    {
    //  if (twinkleStall = 0)
    //  {
        twinkle(hideFullLeds, findFullPallete(fullPalleteMode));
        twinkleStall = 20;
    //  }
    //  else 
    //  {
    //    twinkleStall--;
    //  }
    }
    //else
    //{
    //  twinkleStall = 0;
    //}

    // Final Led Show Logic
    for (int l = 0; l < NUM_LEDS; l++)
    {
      ledsL[l] = (RideLedsL[l]) ? RideLedsL[l] : FullLedsL[l];
      ledsR[l] = (RideLedsR[l]) ? RideLedsR[l] : FullLedsR[l];
    }
    unsigned long delayTime = (strobeMode > 0) ? 50 : 5;
    FastLED.show();
    delay(delayTime);
  }
  else
  { // AUDIO REACTIVE MODE
    clearRGB(RideLedsL);
    clearRGB(RideLedsR);
    delay(1000);
  }
}
