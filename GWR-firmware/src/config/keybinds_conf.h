#pragma once

#include <Arduino.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#include <USBHIDMouse.h>

#include "hardware_conf.h"

// potential todo: put this all in an object with robust getters/setters

// *** AUDIO/IMAGE MODE ***
// allows navigating
// press 1-> select sound code
// press 2-> select image code
// press 5-> play image and sound file simultaneously
// (then a two digit sequence, yielding 49 possible binds per media type)

constexpr uint8_t MM_BUTTON_AUDIO = Buttons::FREN;
constexpr uint8_t MM_BUTTON_IMAGE = Buttons::SHAFT;
constexpr uint8_t MM_BUTTON_BOTH  = Buttons::LFAR;

enum KeybindMode {
    STANDARD,   // the combination just presses the key once
    TOGGLEHOLD, // the combination toggles holding of the key
    INTERACTIVE // the key is held until the physical button is released
};

// *** KEYBOARD MODE ***
// requires build flags set up for USB-OTG
// operates in keybindmode standard
// use scancodes for non-printing keys and modifiers (>= 0x80)
// the character must also be supported by the current font
// 0U signifies an unassigned key
#define KEYBOARDMAP_WIDTH (3)
constexpr uint8_t keyboard_map[][KEYBOARDMAP_WIDTH] = {
    {Buttons::TIP,   Buttons::TIP,   '.'          },
    {Buttons::TIP,   Buttons::FREN,  'Y'          },
    {Buttons::TIP,   Buttons::SHAFT, 'Z'          },
    {Buttons::TIP,   Buttons::LNEAR, 'G'          },
    {Buttons::TIP,   Buttons::RNEAR, 'P'          },
    {Buttons::TIP,   Buttons::LFAR,  0U           },
    {Buttons::TIP,   Buttons::RFAR,  0U           },

    {Buttons::FREN,  Buttons::TIP,   'U'          },
    {Buttons::FREN,  Buttons::FREN,  'E'          },
    {Buttons::FREN,  Buttons::SHAFT, 'I'          },
    {Buttons::FREN,  Buttons::LNEAR, 'A'          },
    {Buttons::FREN,  Buttons::RNEAR, 'O'          },
    {Buttons::FREN,  Buttons::LFAR,  'B'          },
    {Buttons::FREN,  Buttons::RFAR,  'V'          },

    {Buttons::SHAFT, Buttons::TIP,   KEY_BACKSPACE},
    {Buttons::SHAFT, Buttons::FREN,  'R'          },
    {Buttons::SHAFT, Buttons::SHAFT, KEY_SPACE    },
    {Buttons::SHAFT, Buttons::LNEAR, 'L'          },
    {Buttons::SHAFT, Buttons::RNEAR, 'D'          },
    {Buttons::SHAFT, Buttons::LFAR,  'X'          },
    {Buttons::SHAFT, Buttons::RFAR,  'Q'          },

    {Buttons::LNEAR, Buttons::TIP,   'K'          },
    {Buttons::LNEAR, Buttons::FREN,  'S'          },
    {Buttons::LNEAR, Buttons::SHAFT, 'C'          },
    {Buttons::LNEAR, Buttons::LNEAR, 'T'          },
    {Buttons::LNEAR, Buttons::RNEAR, 'M'          },
    {Buttons::LNEAR, Buttons::LFAR,  '!'          },
    {Buttons::LNEAR, Buttons::RFAR,  '?'          },

    {Buttons::RNEAR, Buttons::TIP,   'J'          },
    {Buttons::RNEAR, Buttons::FREN,  'H'          },
    {Buttons::RNEAR, Buttons::SHAFT, 'W'          },
    {Buttons::RNEAR, Buttons::LNEAR, 'F'          },
    {Buttons::RNEAR, Buttons::RNEAR, 'N'          },
    {Buttons::RNEAR, Buttons::LFAR,  0U           },
    {Buttons::RNEAR, Buttons::RFAR,  KEY_RETURN   },

    {Buttons::LFAR,  Buttons::TIP,   '1'          },
    {Buttons::LFAR,  Buttons::FREN,  '2'          },
    {Buttons::LFAR,  Buttons::SHAFT, '3'          },
    {Buttons::LFAR,  Buttons::LNEAR, '4'          },
    {Buttons::LFAR,  Buttons::RNEAR, 0U          },
    {Buttons::LFAR,  Buttons::LFAR,  0U           },
    {Buttons::LFAR,  Buttons::RFAR,  0U           },

    {Buttons::RFAR,  Buttons::TIP,   '9'          },
    {Buttons::RFAR,  Buttons::FREN,  '8'          },
    {Buttons::RFAR,  Buttons::SHAFT, '7'          },
    {Buttons::RFAR,  Buttons::LNEAR, '6'          },
    {Buttons::RFAR,  Buttons::RNEAR, '5'           },
    {Buttons::RFAR,  Buttons::LFAR,  0U           },
    {Buttons::RFAR,  Buttons::RFAR,  '0'          },
};

constexpr uint32_t KEYBOARDMAP_LENGTH = sizeof(keyboard_map) / KEYBOARDMAP_WIDTH;

// *** JOYSTICK MODE ***
#define JOYSTICKMAP_SINGLE_WIDTH (2)

constexpr uint8_t joystick_single_keyMap[][JOYSTICKMAP_SINGLE_WIDTH] = {
    {Buttons::FREN,  'W'      },
    {Buttons::SHAFT, KEY_SPACE},
    {Buttons::LNEAR, 'A'      },
    {Buttons::RNEAR, 'D'      },
};

constexpr uint8_t joystick_single_mouseMap[][JOYSTICKMAP_SINGLE_WIDTH] = {
    {Buttons::LFAR, MOUSE_LEFT },
    {Buttons::RFAR, MOUSE_RIGHT},
};

#define JOYSTICKMAP_DUAL_WIDTH (4)
constexpr uint8_t joystick_dual_map[][JOYSTICKMAP_DUAL_WIDTH] = {
    // button1, button2, key to press, press mode
    // ! only works with keyboard for now (e.g. no middle click via a 2-button keybind)
    {Buttons::TIP, Buttons::TIP,   'S',            KeybindMode::INTERACTIVE},
    {Buttons::TIP, Buttons::FREN,  'R',            KeybindMode::STANDARD   },
    {Buttons::TIP, Buttons::SHAFT, 'Q',            KeybindMode::STANDARD   },
    {Buttons::TIP, Buttons::LNEAR, 'F',            KeybindMode::STANDARD   },
    {Buttons::TIP, Buttons::RNEAR, 'E',            KeybindMode::STANDARD   },
    {Buttons::TIP, Buttons::LFAR,  KEY_LEFT_SHIFT, KeybindMode::TOGGLEHOLD },
    {Buttons::TIP, Buttons::RFAR,  KEY_TAB,        KeybindMode::INTERACTIVE},
};

constexpr uint32_t JOYSTICKMAP_SINGLEKEY_LENGTH   = sizeof(joystick_single_keyMap) / JOYSTICKMAP_SINGLE_WIDTH;
constexpr uint32_t JOYSTICKMAP_SINGLEMOUSE_LENGTH = sizeof(joystick_single_mouseMap) / JOYSTICKMAP_SINGLE_WIDTH;
constexpr uint32_t JOYSTICKMAP_DUAL_LENGTH        = sizeof(joystick_dual_map) / JOYSTICKMAP_DUAL_WIDTH;