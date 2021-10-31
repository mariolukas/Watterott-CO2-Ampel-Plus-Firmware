FORMAT_FILES=$(shell find CO2-Ampel_Plus -iname '*.h' -print0 -or -iname '*.cpp' -print0 | xargs -0 echo)
FORMAT_COMMAND="--style=Chromium -verbose -i $(FORMAT_FILES)"
FORMAT_TEST_COMMAND="--style=Chromium -verbose --Werror --dry-run $(FORMAT_FILES)"

.PHONY: clean build-builder build

clean:
	@rm -rf build

build-builder:
	docker-compose build arduino-builder-co2ampel

build-builder-formatter:
	docker-compose build arduino-builder-co2ampel-format

build: build-builder
	docker-compose up arduino-builder-co2ampel
	docker cp co2ampel-builder:/usr/src/app/build ./

format: build-builder-formatter
	FORMAT_COMMAND=$(FORMAT_COMMAND) USER="$(shell id -u):$(shell id -g)" docker-compose up arduino-builder-co2ampel-format

format-test: build-builder-formatter
	FORMAT_COMMAND=$(FORMAT_TEST_COMMAND)  USER="$(shell id -u):$(shell id -g)" docker-compose up arduino-builder-co2ampel-format
