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

#include <Arduino.h>

// *** KEYBOARD MODE ***
// separating the mapping into separate arrays like this
// helps performance (its like a hashmap kinda idk)
// '<' = backspace, '>' = TAB hold, '^' = SHIFT toggle

// endswith:   ......   0    1    2    3    4    5    6
char startswith_0[] = {'.', 'Y', 'Z', 'G', 'P', '\0', '\0'};
char startswith_1[] = {'U', 'E', 'I', 'A', 'O', 'B', 'V'};
char startswith_2[] = {'<', 'R', ' ', 'L', 'D', '\0', '\0'};
char startswith_3[] = {'K', 'S', 'C', 'T', 'M', '!', '?'};
char startswith_4[] = {'J', 'H', 'W', 'F', 'N', '\0', '\0'};
char startswith_5[] = {'\0', 'X', '\0', '\0', '\0', '\0', '\0'};
char startswith_6[] = {'\0', 'Q', '\0', '\0', '\0', '\0', '\0'};

// *** JOYSTICK MODE ***
// gaming mode          0    1    2    3    4
char single_press[] = {'A', 'W', ' ', 'S', 'D'};
char startswith_6_gaming[] = {'~', '~', '~', '~', '~', '^', 'E'};
char startswith_7_gaming[] = {'>', '1', '2', '3', '4', 'Q', 'F'};


// *** AUDIO/IMAGE MODE ***
// basically navigate folders.
// press 1-> select sound code
// press 2-> select image code
// (then a two digit sequence, yields 49 binds per media type)