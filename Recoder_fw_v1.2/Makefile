
DEVICE	= atmega8
CFLAGS  = -DDEBUG_LEVEL=0
F_CPU   = 2000000	# in Hz
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)


all: 
	$(COMPILE) main.c -o main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-objdump -h -S main.elf > main.lss
	avr-size main.hex


