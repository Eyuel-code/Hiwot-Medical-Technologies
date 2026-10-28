TARGET = hiwot_firmware
MCU = atmega328p
F_CPU = 16000000UL

CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -std=c99 -Wall -Wextra
LDFLAGS = -mmcu=$(MCU) -lm

SRCS = src/main.c src/lcd.c src/adc.c src/indicators.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET).hex

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET).elf $(TARGET).hex

flash: $(TARGET).hex
	avrdude -c usbasp -p m328p -U flash:w:$(TARGET).hex:i

.PHONY: all clean flash