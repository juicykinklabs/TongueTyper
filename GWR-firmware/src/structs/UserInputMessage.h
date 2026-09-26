#pragma once

#include <Arduino.h>
// user input events come from pressing buttons on the tonguetyper.
// when a button is pushed, multiple events may be placed in queue. some events
// will be ignored depending on mode
// this is BEFORE passing through the keybinds_conf.h layer, which task_mode_keyboard handles
// userInputEvents are consumed and piped to the keyboard or mouse task, display task, audio task, or whatever else
// buttons.cpp handles all the timing and debouncing, we get a stream of actionable inputs
// in the current architecture, buttson.cpp handles the hardware conf, which decides which pins are actually buttons
// and maps physical to logical IDs

enum UserInputEnum {
    NONE,    // the message should be ignored by recipient; something happened to the button, but it's not considered valid
    SINGLE,  // used for gaming/joystick mode (button1) (2/3 are dontcare)
    PAIR,    // used for typing (button1+button2) (3 is dontcare)
    TRIPLET, // used by display/audio tasks (button1+button2+button3)
    RELEASE  // a button has been released (button1) (2/3 are dontcare)
};

typedef struct UserInputMessage {
    UserInputEnum uit;
    uint8_t button1;
    uint8_t button2;
    uint8_t button3;
} UserInputMessage;