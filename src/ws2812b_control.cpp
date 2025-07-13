#include <Arduino.h>
#include <stdio.h>
#include <Adafruit_NeoPixel.h>

#include "definitions.h"
#include "ws2812b_control.h"



// Define the LED indices for each segment (A-G), with 2 LEDs per segment
const int SEGMENT_LEDS[7][2] = {
    {8, 9},     // Segment A
    {10, 11},   // Segment B
    {0, 1},     // Segment C
    {2, 3},     // Segment D
    {4, 5},     // Segment E
    {6, 7},     // Segment f
    {12, 13}    // Segment G
};

// Lookup table for digits (0-9) using segments A-G
const uint8_t DIGIT_SEGMENTS[18] = {
    0b1111110, // 0: Segments A, B, C, D, E, F 
    0b0110000, // 1: Segments B, C
    0b1101101, // 2: Segments A, B, D, E, G
    0b1111001, // 3: Segments A, B, C, D, G
    0b0110011, // 4: Segments B, C, F, G
    0b1011011, // 5: Segments A, C, D, F, G
    0b1011111, // 6: Segments A, C, D, E, F, G
    0b1110000, // 7: Segments A, B, C
    0b1111111, // 8: Segments A, B, C, D, E, F, G
    0b1111011, // 9: Segments A, B, C, D, F, G
    0b0000001, //10: -: Segments E, F, G (for the dash)
    0b1001110, //11: C: Segments A, D, E, F
    0b0010101, //12: n: Segments A, B, C, D, E, F (for the letter 'n')
    0b1000111, //13: F: Segments A, B, E, F (for the letter 'F')
    0b1110111, //14: A: Segments A, B, C, E, F (for the letter 'A')
    0b0001110, //15: L: Segments B, C, D, E (for the letter 'L')
    0b1100111, //16: P: Segments A, B, C, D, E, F (for the letter 'P')
    0b0000001  //17: -: Segments E, F, G (for the dash)


};




Adafruit_NeoPixel strip(LED_NUM, GPIO_LED, NEO_GRB + NEO_KHZ800);


void led_init() {
    strip.begin();

    strip.setBrightness(LED_BRIGHTNESS); // Set brightness of the strip

    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0); // Turn off all LEDs
    }

    strip.show(); // Initialize all LEDs to 'off'
    Serial.println("LED strip initialized.");
}

// Function to display symbols on the LED strip
void led_display_symbol(uint8_t position, int8_t digit, uint32_t color) {
    //if (digit < -1 || digit > 9) return; // Invalid digit
    if (position < 0 || position > 3) return; // Invalid position

    uint8_t segments = DIGIT_SEGMENTS[digit];
    // Convert color from 0xRRGGBB format to strip.Color(RR, GG, BB)
    uint8_t red = (color >> 16) & 0xFF;
    uint8_t green = (color >> 8) & 0xFF;
    uint8_t blue = color & 0xFF;
    color = strip.Color(red, green, blue);

    if (digit == -1) {
        // If digit is -1, turn off all segments for this position
        segments = 0;
    }

    // Offset the segment indices based on the digit position
    int offset = (position) * 14; // Each digit uses 14 LEDs (7 segments * 2 LEDs per segment)

    // Clear all LEDs
    //strip.clear();

    // Light up the LEDs for the active segments
    for (int i = 0; i < 7; i++) { // Loop through segments A-G (0-6)
        if (segments & (1 << (6-i))) { // Check if the segment is active
            for (int j = 0; j < 2; j++) { // Loop through both LEDs in the segment
                strip.setPixelColor(offset + SEGMENT_LEDS[i][j], color); // Set color for the LED
            }
        } else {
            // Turn off the LEDs for the inactive segments
            for (int j = 0; j < 2; j++) {
                strip.setPixelColor(offset + SEGMENT_LEDS[i][j], 0); // Set color to off
            }
        }
    }

    strip.show(); // Update the display
}

void led_display_digit(uint8_t position, int8_t digit, uint32_t color) {
    if (digit < -1 || digit > 9) return; // Invalid digit
    if (position < 0 || position > 3) return; // Invalid position

    led_display_symbol(position, digit, color);
}

void led_test() {
    Serial.println("Testing LED strip...");
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 0)); // Red
    }
    strip.show();
    delay(500);
    
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 255, 0)); // Green
    }
    strip.show();
    delay(500);
    
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 255)); // Blue
    }
    strip.show();
    delay(300);
    
    led_init(); // Reset to off state
}

void led_digit_test() {
    Serial.println("Testing digit display...");

    for (int digit = 0; digit < 10; digit++) {
        led_display_digit(0, digit, 0xFF0000); // Display each digit in yellow
        led_display_digit(1, digit, 0x00FF00); // Display each digit in yellow
        led_display_digit(2, digit, 0x0000FF); // Display each digit in yellow
        led_display_digit(3, digit, 0xFFFFFF); // Display each

        delay(1000); // Wait for a second

    }

    led_init(); // Reset to off state
}


void led_display_word_COn(bool show_dash) {
    Serial.println("Displaying word 'COn-'...");

    // Display 'C' on the first segment
    led_display_symbol(3, 11, 0xFFFFFF); // Assuming '11' represents 'C' in your digit mapping

    // Display 'O' on the second segment
    led_display_symbol(2, 0, 0xFFFFFF); // '0' represents 'O'

    // Display 'n' on the third segment
    led_display_symbol(1, 12, 0xFFFFFF); // Assuming '12' represents 'n' in your digit mapping

    // Display '-' on the fourth segment based on the input argument
    if (show_dash) {
        led_display_symbol(0, 10, 0xFFFFFF); // Assuming '10' represents '-'
    } else {
        led_display_symbol(0, -1, 0xFFFFFF); // Turn off the fourth segment
    }
}


void led_display_word_FAIL(uint8_t seconds) {
    Serial.println("Displaying word 'FAIL'...");

    // Display 'F' on the first segment
    led_display_symbol(3, 13, 0xFF0000); // Assuming '13' represents 'F' in your digit mapping

    // Display 'A' on the second segment
    led_display_symbol(2, 14, 0xFF0000); // '14' represents 'A'

    // Display 'I' on the third segment
    led_display_symbol(1, 1, 0xFF0000); // Assuming '1' represents 'I'

    // Display 'L' on the fourth segment
    led_display_symbol(0, 15, 0xFF0000); // Assuming '15' represents 'L'

    if (seconds == 0) {
        while(1); // Ensure at least 1 second for display
    }
    delay(1000 * seconds); // Refresh every second

    led_init(); // Reset to off state
}

void led_display_word_AP(uint8_t num) {
    if (num > 99) return; // Limit to two digits

    Serial.println("Displaying word 'AP'...");
    // Display 'A' on the first segment
    led_display_symbol(3, 14, 0xFFFFFF); // Assuming '14' represents 'A' in your digit mapping

    // Display 'P' on the second segment
    led_display_symbol(2, 16, 0xFFFFFF); // Assuming '16' represents 'P' in your digit mapping

    // Display the number on the third and fourth segments
    uint8_t lower_digit = num % 10; // Get the lower digit
    uint8_t upper_digit = num / 10; // Get the upper digit

    led_display_symbol(0, lower_digit, 0xFFFFFF); // Display lower digit
    led_display_symbol(1, upper_digit, 0xFFFFFF); // Display upper digit
}

void led_display_ok_dashes() {
    led_display_symbol(0, 17, 0x00FF00); // Display dash in green
    led_display_symbol(1, 17, 0x00FF00); // Display dash in green
    led_display_symbol(2, 17, 0x00FF00); // Display dash in green
    led_display_symbol(3, 17, 0x00FF00); // Display dash in green
}



void led_tmp() {
    Serial.println("Temporary LED test...");

    for (int i = 0; i < 10; i++) {
        led_display_digit(3, i, 0xFF00FF); // Display each digit in yellow
        delay(1000); // Wait for a second
    }

}


