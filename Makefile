PROJECT = CO2-Ampel_Plus
BOARD_TYPE = co2ampel:samd:sb
ARDUINO_CLI = arduino-cli
SERIAL_PORT = COM18
VERBOSE = 1

# Build path -- used to store built binary and object files
BUILD_DIR=_build
BUILD_CACHE_DIR=_build_cache
BUILD_PATH=$(PROJECT)/$(BUILD_DIR)
BUILD_CACHE_PATH=$(PROJECT)/$(BUILD_CACHE_DIR)
BUILD_COMMAND=$(ARDUINO_CLI) compile $(VERBOSE_FLAG) --warnings more --build-path=$(BUILD_PATH) --build-cache-path=$(BUILD_CACHE_PATH) -b $(BOARD_TYPE) $(PROJECT)
CLEAN_COMMAND=rm -rf $(BUILD_PATH)

ifneq ($(V), 0)
	VERBOSE_FLAG=-v
else
	VERBOSE_FLAG=
endif

.PHONY: all example program clean

all: example

builder:
	docker-compose build

clean:
	docker-compose run arduino-builder $(CLEAN_COMMAND)

build: builder clean
	docker-compose run arduino-builder $(BUILD_COMMAND)

# program:
# 	$(ARDUINO_CLI) upload $(VERBOSE_FLAG) -p $(SERIAL_PORT) --fqbn $(BOARD_TYPE) $(PROJECT)
