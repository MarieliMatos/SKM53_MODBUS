
CC = avr-gcc
INCLUDES = -Ilib -I/usr/lib/avr/include -I/usr/lib/gcc/avr/5.4.0/include	\
-I/usr/lib/gcc/avr/5.4.0/include-fixed
CFLAGS := -Wall -Os -fpack-struct -fshort-enums -ffunction-sections	\
-fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields			\
-mmcu=atmega328p -DF_CPU=8000000UL -MMD -MP $(INCLUDES)
LDLIBS = 
LDFLAGS = -mmcu=atmega328p

TARGET = output.elf

MAIN = ./main.c

SRC := $(shell find . -name '*.c')
EXCLUDE := $(shell find . -name '*main*.c')
SRC := $(filter-out $(EXCLUDE), $(SRC))
SRC :=  $(SRC) $(MAIN)

OBJ := $(SRC:.c=.o) ## xx.c yy.c ... -> xx.o yy.o ...

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

#$(TARGET): $(MAIN)
#	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

run: $(OBJ)
	$(CC) -c -o $@ $< $(CFLAGS) # do not link


.PHONY: clean

clean:
	rm -f $(OBJ) $(TARGET)

gtags:
	$(shell export GTAGSLIBPATH=/usr/lib/avr/include:/usr/lib/gcc/avr/5.4.0/include:/usr/lib/gcc/avr/5.4.0/include-fixed)
