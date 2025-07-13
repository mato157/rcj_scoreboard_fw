#ifndef WS2812B_CONTROL_H
#define WS2812B_CONTROL_H


void led_init();

void led_display_digit(uint8_t position, int8_t digit, uint32_t color);


void led_test();

void led_digit_test();


void led_display_word_COn(bool show_dash);

void led_display_word_FAIL(uint8_t seconds);
void led_display_word_AP(uint8_t num);
void led_display_ok_dashes();

void led_tmp();


#endif // WS2812B_CONTROL_H