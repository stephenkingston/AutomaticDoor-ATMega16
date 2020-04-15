MAKE_HEX = avr-objcopy
CC = avr-gcc

OUTPUT = ATMega16_AutoDoor

SRCS += main.c

INCLUDE +=

CFLAGS += -Wall -g -O2 -mmcu=atmega16
BUILD_DIR = build

C_FILES = $(notdir $(SRCS))
C_PATHS = $(dir $(SRCS))
vpath %.c $(C_PATHS)


HEX = $(BUILD_DIR)/$(OUTPUT).hex
BIN = $(BUILD_DIR)/$(OUTPUT).bin


####### TARGETS ########

all: $(BUILD_DIR) $(BIN) $(HEX)


$(HEX): $(BIN)
	@$(MAKE_HEX) -j .text -j .data -O ihex $(BIN) $(HEX)

$(BUILD_DIR):
	@mkdir "$@"

$(BIN): $(C_FILES) $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $(addprefix -I,$(INCLUDE)) $(C_FILES)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.lst *.rst *.o *.bin
	
.PHONY: flash
flash: $(HEX)
	avrdude -p atmega16 -c usbasp -U flash:w:$(HEX):i -F -P usb