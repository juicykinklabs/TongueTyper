#pragma once

#include <Arduino.h>

#include <USB.h>
#include <USBHIDKeyboard.h>
#include <USBHIDMouse.h>

#define JOYSTICKMODE_STANDARD    (0) // the combination just presses the key once
#define JOYSTICKMODE_TOGGLEHOLD  (1) // the combination toggles holding of the key
#define JOYSTICKMODE_INTERACTIVE (2) // the key is held until the physical button is released

// *** KEYBOARD MODE ***
// requires build flags set up for USB-OTG

// mapping for each button onto a gpio expander port and bit
// port A: just the bit number
// port B: bit is &'d with 0x80
constexpr uint8_t TIP   = 7;
constexpr uint8_t FREN  = 3;
constexpr uint8_t SHAFT = 1;
constexpr uint8_t LNEAR = 4;
constexpr uint8_t RNEAR = 6;
constexpr uint8_t LFAR  = 0;
constexpr uint8_t RFAR  = 2;

// example extension:
#define HAS_REVB_DAUGHTERBOARD (1)
constexpr uint8_t TOPP = 0 | 0x80; // top button
constexpr uint8_t SHF2 = 1 | 0x80; // second shaft button
constexpr uint8_t LMID = 2 | 0x80; // left-middle button
constexpr uint8_t RMID = 3 | 0x80; // right-middle button

// use scancodes for non-printing keys and modifiers (>= 0x80)
// 0U signifies an unassigned key

uint8_t keyboard_map[][3] = {
    {TIP,   TIP,   '.'          },
    {TIP,   FREN,  'Y'          },
    {TIP,   SHAFT, 'Z'          },
    {TIP,   LNEAR, 'G'          },
    {TIP,   RNEAR, 'P'          },
    {TIP,   LFAR,  0U           },
    {TIP,   RFAR,  0U           },

    {FREN,  TIP,   'U'          },
    {FREN,  FREN,  'E'          },
    {FREN,  SHAFT, 'I'          },
    {FREN,  LNEAR, 'A'          },
    {FREN,  RNEAR, 'O'          },
    {FREN,  LFAR,  'B'          },
    {FREN,  RFAR,  'V'          },

    {SHAFT, TIP,   KEY_BACKSPACE},
    {SHAFT, FREN,  'R'          },
    {SHAFT, SHAFT, KEY_SPACE    },
    {SHAFT, LNEAR, 'L'          },
    {SHAFT, RNEAR, 'D'          },
    {SHAFT, LFAR,  'X'          },
    {SHAFT, RFAR,  'Q'          },

    {LNEAR, TIP,   'K'          },
    {LNEAR, FREN,  'S'          },
    {LNEAR, SHAFT, 'C'          },
    {LNEAR, LNEAR, 'T'          },
    {LNEAR, RNEAR, 'M'          },
    {LNEAR, LFAR,  '!'          },
    {LNEAR, RFAR,  '?'          },

    {RNEAR, TIP,   'J'          },
    {RNEAR, FREN,  'H'          },
    {RNEAR, SHAFT, 'W'          },
    {RNEAR, LNEAR, 'F'          },
    {RNEAR, RNEAR, 'N'          },
    {RNEAR, LFAR,  0U           },
    {RNEAR, RFAR,  KEY_RETURN   },

    {LFAR,  TIP,   0U           },
    {LFAR,  FREN,  0U           },
    {LFAR,  SHAFT, 0U           },
    {LFAR,  LNEAR, 0U           },
    {LFAR,  RNEAR, 0U           },
    {LFAR,  LFAR,  0U           },
    {LFAR,  RFAR,  0U           },

    {RFAR,  TIP,   0U           },
    {RFAR,  FREN,  0U           },
    {RFAR,  SHAFT, 0U           },
    {RFAR,  LNEAR, 0U           },
    {RFAR,  RNEAR, 0U           },
    {RFAR,  LFAR,  0U           },
    {RFAR,  RFAR,  0U           },
};

// *** JOYSTICK MODE ***

uint8_t joystick_single_keyMap[][2] = {
    {FREN,  'W'      },
    {SHAFT, KEY_SPACE},
    {LNEAR, 'A'      },
    {RNEAR, 'D'      },
};

uint8_t joystick_single_mouseMap[][2] = {
    {LFAR, MOUSE_LEFT },
    {RFAR, MOUSE_RIGHT},
};

uint8_t joystick_dual_map[][4] = {
    // button1, button2, key to press, press mode
    {TIP, TIP,   'S',            JOYSTICKMODE_STANDARD   },
    {TIP, FREN,  'R',            JOYSTICKMODE_STANDARD   },
    {TIP, SHAFT, 'Q',            JOYSTICKMODE_STANDARD   },
    {TIP, LNEAR, 'F',            JOYSTICKMODE_STANDARD   },
    {TIP, RNEAR, 'E',            JOYSTICKMODE_STANDARD   },
    {TIP, LFAR,  KEY_LEFT_SHIFT, JOYSTICKMODE_TOGGLEHOLD },
    {TIP, RFAR,  KEY_TAB,        JOYSTICKMODE_INTERACTIVE},
};

// *** AUDIO/IMAGE MODE ***
// basically navigate folders.
// press 1-> select sound code
// press 2-> select image code
// press 5-> play image and sound file simultaneously (todo in future)
// (then a two digit sequence, yields 49 binds per media type)

constexpr uint8_t MM_BUTTON_AUDIO = FREN;
constexpr uint8_t MM_BUTTON_IMAGE = SHAFT;
constexpr uint8_t MM_BUTTON_BOTH  = LFAR;