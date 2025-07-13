#include <Arduino.h>

#include "ws2812b_control.h"
#include "team.h"

Team team1(0); // Create team1 with its pin and color
Team team2(1); // Create team2 with its pin and color

// Implementation of the Team class
Team::Team(uint8_t initPosition)
    : position(initPosition), score(0), color(0xFFFFFF) {}

void Team::setScore(uint8_t newScore) {
    if (newScore > 99) {
        newScore = 99; // Limit score to a maximum of 99
    }
    score = newScore;

    uint8_t lower_digit = score % 10; // Get the lower digit
    uint8_t upper_digit = score / 10; // Get the upper digit
    if (upper_digit == 0) {
        if (position == 0) {
            upper_digit = -1; // If score is less than 10, set upper digit to -1 for team 1
        } else {
            upper_digit = lower_digit; 
            lower_digit = -1; // If score is less than 10, set lower digit to -1 for team 2
        }
    }

    led_display_digit(2 - 2*position, lower_digit, color); // Display lower digit
    led_display_digit(2 - 2*position + 1, upper_digit, color); // Display upper digit

}

void Team::setColor(uint32_t newColor) {
    color = newColor;
}