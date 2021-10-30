PROJECT = CO2-Ampel_Plus
BOARD_TYPE = co2ampel:samd:sb
ARDUINO_CLI = arduino-cli
ARDUINO_CLI_DIR = /usr/bin
SERIAL_PORT = COM18
VERBOSE = 1

# Build path -- used to store built binary and object files
BUILD_DIR=_build
BUILD_CACHE_DIR=_build_cache
BUILD_PATH=$(PWD)/$(PROJECT)/$(BUILD_DIR)
BUILD_CACHE_PATH=$(PWD)/$(PROJECT)/$(BUILD_CACHE_DIR)

ifneq ($(V), 0)
	VERBOSE_FLAG=-v
else
	VERBOSE_FLAG=
endif

.PHONY: all example program clean

all: example

build:
	$(ARDUINO_CLI_DIR)/$(ARDUINO_CLI) compile $(VERBOSE_FLAG) --build-path=$(BUILD_PATH) --build-cache-path=$(BUILD_CACHE_PATH) -b $(BOARD_TYPE) $(PROJECT)

program:
	$(ARDUINO_CLI_DIR)/$(ARDUINO_CLI) upload $(VERBOSE_FLAG) -p $(SERIAL_PORT) --fqbn $(BOARD_TYPE) $(PROJECT)

clean:
	-@rm -rf $(BUILD_PATH)
