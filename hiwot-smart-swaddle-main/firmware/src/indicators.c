#include "indicators.h"

void indicators_init(void) {
    DDRB |= (1 << LED_APNEA_PIN) | (1 << LED_POWER_PIN) | 
            (1 << LED_OFFLINE_PIN) | (1 << BUZZER_PIN);
    PORTB &= ~((1 << LED_APNEA_PIN) | (1 << LED_POWER_PIN) | 
               (1 << LED_OFFLINE_PIN) | (1 << BUZZER_PIN));
}

void set_led(uint8_t pin, uint8_t state) {
    if (state) PORTB |= (1 << pin);
    else PORTB &= ~(1 << pin);
}

void trigger_buzzer(uint8_t state) {
    if (state) PORTB |= (1 << BUZZER_PIN);
    else PORTB &= ~(1 << BUZZER_PIN);
}