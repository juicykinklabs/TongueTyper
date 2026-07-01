// gagwriter keybinds/mappings
// 2-button sequences can be converted to uhhh like a letter or symbol.
// generated this shi in excel with a bunch of heuristics and stuf

// buttons:
// 0: tip
// 1: fren
// 2: shaft
// 3: Left Near
// 4: Right Near
// 5: Left Far
// 6: Right Far
#pragma once

#include <Arduino.h>

#include <USB.h>
#include <USBHIDKeyboard.h>

// *** KEYBOARD MODE ***
// requires build flags set up for USB-OTG

// separating the mapping into separate arrays like this
// helps performance (its like a hashmap kinda idk)
// use scancodes (8 bits) for non-printing keys and modifiers (>= 0x80)
// use 0U for unassigned keys

// endswith:   ......      0    1    2    3    4    5    6

uint8_t startswith_0[] = {'.', 'Y', 'Z', 'G', 'P', 0U, 0U};
uint8_t startswith_1[] = {'U', 'E', 'I', 'A', 'O', 'B', 'V'};
uint8_t startswith_2[] = {KEY_BACKSPACE, 'R', KEY_SPACE, 'L', 'D', 0U, 0U};
uint8_t startswith_3[] = {'K', 'S', 'C', 'T', 'M', '!', '?'};
uint8_t startswith_4[] = {'J', 'H', 'W', 'F', 'N', 0U, KEY_RETURN};
uint8_t startswith_5[] = {0U, 'X', 0U, 0U, 0U, 0U, 0U};
uint8_t startswith_6[] = {0U, 'Q', 0U, 0U, 0U, 0U, 0U};

// *** JOYSTICK MODE ***
// gaming mode             0    1    2    3    4
uint8_t single_press[] = {'A', 'W', KEY_SPACE, 'S', 'D'};
uint8_t startswith_6_gaming[] = {0U, 0U, 0U, 'R', 'E', KEY_LEFT_SHIFT, 0U};
uint8_t startswith_7_gaming[] = {KEY_TAB, '1', '2', '3', '4', 'Q', 'F'};


// *** AUDIO/IMAGE MODE ***
// basically navigate folders.
// press 1-> select sound code
// press 2-> select image code
// (then a two digit sequence, yields 49 binds per media type)