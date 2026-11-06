#include <stdint.h>

// ============================================================
// BEGIN TEENSY DEFINITIONS
// ============================================================

#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT_PULLUP 2

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
uint8_t digitalRead(uint8_t pin);
void delay(uint32_t milliseconds);

// ============================================================
// END TEENSY DEFINITIONS - game code starts below
// ============================================================

// Pin assignments: Teensy 4.1 board pin numbers
#define LED0 11
#define LED1 10
#define LED2 12
#define LED3 14
#define LED5 5   // Red: wrong answer
#define LED6 6   // Green: win

#define BTN0 9
#define BTN1 2
#define BTN2 3
#define BTN3 4

int led_pins[4] = {LED0, LED1, LED2, LED3};
int btn_pins[4] = {BTN0, BTN1, BTN2, BTN3};

// 0-3 select the matching game LED and button.
int sequence[12] = {2, 1, 0, 1, 3, 3, 2, 1, 0, 2, 3, 0};

// delay() is supplied by the Teensy core, in milliseconds.

void led_on(int pin) {
    digitalWrite(pin, HIGH);
}

void led_off(int pin) {
    digitalWrite(pin, LOW);
}

// Buttons connect between their pin and GND.
int press_button(int pin) {
    return digitalRead(pin) == LOW;
}

void display_sequence(int length) {
    for (int i = 0; i < length; i++) {
        int led = led_pins[sequence[i]];
        led_on(led);
        delay(500);
        led_off(led);
        delay(500);
    }
}

int get_button_press(void) {
    int pressed = -1;

    while (pressed == -1) {
        if (press_button(BTN0)) pressed = 0;
        else if (press_button(BTN1)) pressed = 1;
        else if (press_button(BTN2)) pressed = 2;
        else if (press_button(BTN3)) pressed = 3;
    }

    int led = led_pins[pressed];
    int btn_pin = btn_pins[pressed];

    led_on(led);

    // Wait for release so a held button is counted once.
    // No debouncing is performed.
    while (press_button(btn_pin));

    led_off(led);
    return pressed;
}

int get_player_input(int length) {
    for (int i = 0; i < length; i++) {
        int pressed = get_button_press();
        if (pressed != sequence[i]) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    // Configure pins using the Teensy core's C functions.
    for (int i = 0; i < 4; i++) {
        pinMode(led_pins[i], OUTPUT);
        pinMode(btn_pins[i], INPUT_PULLUP);
        led_off(led_pins[i]);
    }

    pinMode(LED5, OUTPUT);
    pinMode(LED6, OUTPUT);
    led_off(LED5);
    led_off(LED6);

    while (1) {
        int current_length = 2;

        while (current_length <= 12) {
            display_sequence(current_length);
            delay(500);

            int correct = get_player_input(current_length);

            if (correct) {
                current_length++;
            } else {
                // Red stays on while the game LEDs flash twice.
                led_on(LED5);

                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 4; j++) {
                        led_on(led_pins[j]);
                    }
                    delay(500);

                    for (int j = 0; j < 4; j++) {
                        led_off(led_pins[j]);
                    }
                    delay(500);
                }

                led_off(LED5);
                current_length = 2;
            }

            if (current_length <= 12) {
                delay(500);
            }
        }

        // Win: all other LEDs off, green on for two seconds.
        for (int i = 0; i < 4; i++) {
            led_off(led_pins[i]);
        }
        led_off(LED5);
        led_on(LED6);
        delay(2000);
        led_off(LED6);

        // The outer loop restarts the game at two steps.
    }
}