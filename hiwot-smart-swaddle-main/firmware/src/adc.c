#include "adc.h"

void adc_init(void) {
    ADMUX = (1 << REFS0); 
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); 
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    _delay_us(10);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    (void)ADC; // Dummy read
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

float adc_to_tempC(uint16_t raw) {
    if (raw == 0) raw = 1;
    float resistance = SERIES_RESISTOR * (1023.0 / raw - 1.0);
    float steinhart = resistance / NTC_NOMINAL_RES;
    steinhart = log(steinhart);
    steinhart /= NTC_BETA;
    steinhart += 1.0 / (25.0 + 273.15);
    steinhart = 1.0 / steinhart;
    steinhart -= 273.15;
    return steinhart;
}