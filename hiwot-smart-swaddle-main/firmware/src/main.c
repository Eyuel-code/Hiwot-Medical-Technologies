#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcd.h"
#include "adc.h"
#include "indicators.h"

// ================= UART Setup =================
#define BAUD 9600
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

void uart_init(void) {
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)UBRR_VALUE;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_tx_char(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_tx_string(const char* s) {
    while (*s) uart_tx_char(*s++);
}

// ================= ESP8266 Setup =================
// WIFI_SSID, WIFI_PASSWORD and THINGSPEAK_WRITE_API_KEY live in secrets.h (not committed).
// Copy secrets.h.example to secrets.h and fill in your own values.
#include "secrets.h"
#define THINGSPEAK_UPDATE_INTERVAL_MS 20000UL

void esp_init(void) {
    uart_tx_string("AT+CWMODE=1\r\n");
    _delay_ms(500);
    char joinCmd[96];
    snprintf(joinCmd, sizeof(joinCmd), "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWORD);
    uart_tx_string(joinCmd);
    _delay_ms(5000);
}

void esp_push_telemetry(float skinTempC, float bpm, uint8_t alarmFlag) {
    int tempWhole = (int)skinTempC;
    int tempFrac  = (int)(fabs(skinTempC - tempWhole) * 100);
    char cmd[160];
    snprintf(cmd, sizeof(cmd),
        "AT+HTTPCLIENT=2,0,\"http://api.thingspeak.com/update?api_key=%s"
        "&field1=%d.%02d&field2=%d&field3=%d\",,1\r\n",
        THINGSPEAK_WRITE_API_KEY,
        tempWhole, tempFrac,
        (int)bpm,
        alarmFlag);
    uart_tx_string(cmd);
}

// ================= PWM Setup =================
void pwm_heater_init(void) {
    DDRB |= (1 << PB1);
    TCCR1A = (1 << COM1A1) | (1 << WGM10);
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A = 0;
}

void set_heater_pwm(uint8_t duty) {
    OCR1A = duty;
}

// ================= Constants =================
#define ADC_CH_PIEZO 1
#define ADC_CH_SKIN  0

#define TEMP_TARGET_LOW   36.5
#define TEMP_TARGET_HIGH  37.5
#define TEMP_SOFT_CUTOFF  38.0
#define TEMP_HYPOTHERMIA  36.0

#define RESP_THRESHOLD          30
#define APNEA_TIMEOUT_MS        15000UL
#define BREATH_REFRACTORY_MS    400UL

volatile unsigned long ms_ticks = 0;
void tick(unsigned int ms) {
    ms_ticks += ms;
}

int main(void) {
    adc_init();
    pwm_heater_init();
    uart_init();
    lcd_init();
    indicators_init(); 

    lcd_set_cursor(0, 0);
    lcd_string("Hiwot Bench v1");
    _delay_ms(1000);
    lcd_command(0x01);
    
    esp_init();

    int respBaseline = adc_read(ADC_CH_PIEZO);
    unsigned long lastBreathTime = 0;
    unsigned long lastBreathDetectedAt = 0;
    int breathCount = 0;
    unsigned long breathWindowStart = 0;
    float breathsPerMinute = 0;
    uint8_t aboveThreshold = 0;
    uint8_t apneaAlarm = 0;
    
    char line1[17];
    char line2[17];
    
    unsigned long lastLcdUpdate = 0;
    unsigned long lastThingSpeakUpdate = 0;
    unsigned long lastThermalUpdate = 0;
    
    uint16_t rawSkin = 0;
    float skinTemp = 0;
    uint8_t duty = 0, sensorFault = 0, overTemp = 0, underTemp = 0, alarm = 0;
    
    const unsigned int LOOP_DELAY_MS = 20;
    const unsigned long THERMAL_UPDATE_MS = 250;

    while (1) {
        // ---- Respiration ----
        int rawResp = adc_read(ADC_CH_PIEZO);
        int delta = rawResp - respBaseline;
        
        if (!aboveThreshold && delta > RESP_THRESHOLD) {
            if (ms_ticks - lastBreathTime > BREATH_REFRACTORY_MS) {
                aboveThreshold = 1;
                lastBreathTime = ms_ticks;
                lastBreathDetectedAt = ms_ticks;
                breathCount++;
            }
        } else if (aboveThreshold && delta < (RESP_THRESHOLD / 2)) {
            aboveThreshold = 0;
        }

        if (ms_ticks - breathWindowStart >= 15000UL) {
            breathsPerMinute = (breathCount / 15.0) * 60.0;
            breathCount = 0;
            breathWindowStart = ms_ticks;
        }
        apneaAlarm = (ms_ticks - lastBreathDetectedAt) > APNEA_TIMEOUT_MS;

        // ---- Thermal Control ----
        if (ms_ticks - lastThermalUpdate >= THERMAL_UPDATE_MS) {
            lastThermalUpdate = ms_ticks;
            rawSkin = adc_read(ADC_CH_SKIN);
            skinTemp = adc_to_tempC(rawSkin);
            
            sensorFault = (skinTemp < 0.0 || skinTemp > 50.0);
            
            if (sensorFault) {
                duty = 0;
            } else if (skinTemp >= TEMP_SOFT_CUTOFF) {
                duty = 0;
            } else if (skinTemp < TEMP_TARGET_LOW) {
                float error = TEMP_TARGET_LOW - skinTemp;
                float p = 120.0 + error * 40.0;
                if (p > 255.0) p = 255.0;
                if (p < 0.0) p = 0.0;
                duty = (uint8_t)p;
            } else if (skinTemp > TEMP_TARGET_HIGH) {
                duty = 0;
            } else {
                duty = 60;
            }
            set_heater_pwm(duty);
            
            overTemp = (skinTemp >= TEMP_SOFT_CUTOFF);
            underTemp = (!sensorFault) && (skinTemp < TEMP_HYPOTHERMIA);
            alarm = overTemp || underTemp || apneaAlarm || sensorFault;
        }

        // ---- Update Indicators ----
        if (apneaAlarm) {
            set_led(LED_APNEA_PIN, 1);
            trigger_buzzer(1);
        } else {
            set_led(LED_APNEA_PIN, 0);
            trigger_buzzer(0);
        }

        if (overTemp || underTemp || sensorFault) {
            set_led(LED_POWER_PIN, 1);
        } else {
            set_led(LED_POWER_PIN, 0);
        }
        
        set_led(LED_OFFLINE_PIN, sensorFault); 

        // ---- LCD Update ----
        if (ms_ticks - lastLcdUpdate >= 1000UL) {
            lastLcdUpdate = ms_ticks;
            lcd_set_cursor(0, 0);
            snprintf(line1, sizeof(line1), "S:%2d.%1dC BPM:%3d",
                     (int)skinTemp, (int)(fabs(skinTemp - (int)skinTemp) * 10),
                     (int)breathsPerMinute);
            lcd_string(line1);
            
            lcd_set_cursor(1, 0);
            if (sensorFault) {
                snprintf(line2, sizeof(line2), "SENSOR FAULT");
            } else if (alarm) {
                snprintf(line2, sizeof(line2), "ALARM %s%s%s",
                         overTemp ? "T" : "", underTemp ? "L" : "", apneaAlarm ? "A" : "");
            } else {
                snprintf(line2, sizeof(line2), "Breathing Check");
            }
            lcd_string(line2);
        }

        // ---- ESP8266 Telemetry ----
        if (ms_ticks - lastThingSpeakUpdate >= THINGSPEAK_UPDATE_INTERVAL_MS) {
            lastThingSpeakUpdate = ms_ticks;
            esp_push_telemetry(skinTemp, breathsPerMinute, alarm);
        }

        _delay_ms(LOOP_DELAY_MS);
        tick(LOOP_DELAY_MS);
    }
    return 0;
}