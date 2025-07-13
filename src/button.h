// filepath: /home/mato/Documents/PlatformIO/Projects/score_board/src/button.h
#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

// Initializes the button pin
void init_button();

// Handles button press logic (short press, long press, etc.)
void handle_button();

#endif // BUTTON_H