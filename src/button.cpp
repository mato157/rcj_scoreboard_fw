#include <Arduino.h>

#include "button.h"
#include "net.h"
#include "definitions.h"


static uint32_t button_start_time = 0; // Variable to check if long press is detected


void init_button() {
    // init button pin
  pinMode(GPIO_BUTTON, INPUT_PULLUP); // Set the button pin as input with pull
}


void handle_button() {
    if (digitalRead(GPIO_BUTTON) == LOW) { // Check if the button is pressed
        if (button_start_time == 0) {
          button_start_time = millis(); // Start timing when button is pressed
        } else if (millis() - button_start_time > 5000) { // Check if button is pressed for more than 5 seconds
          Serial.println("Button long pressed, starting AP mode...");
          startAPMode(); // Start AP mode if button is long pressed
        }
      } else {
        button_start_time = 0; // Reset the timer if button is released
      }
}