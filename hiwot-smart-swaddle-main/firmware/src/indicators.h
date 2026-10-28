#ifndef INDICATORS_H
#define INDICATORS_H
#include <avr/io.h>

#define LED_APNEA_PIN   PB2
#define LED_POWER_PIN   PB3
#define LED_OFFLINE_PIN PB4
#define BUZZER_PIN      PB5

void indicators_init(void);
void set_led(uint8_t pin, uint8_t state);
void trigger_buzzer(uint8_t state);
#endif