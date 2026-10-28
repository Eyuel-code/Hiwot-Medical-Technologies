#ifndef ADC_H
#define ADC_H
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#define NTC_NOMINAL_RES  10000.0
#define NTC_BETA         3950.0
#define SERIES_RESISTOR  10000.0

void adc_init(void);
uint16_t adc_read(uint8_t channel);
float adc_to_tempC(uint16_t raw);
#endif