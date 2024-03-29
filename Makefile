build_path:=build
release_path:=release

cat := $(if $(filter $(OS),Windows_NT),type,cat)
version_info := $(shell $(cat) VERSION)

project_name := $(shell $(cat) PRJNAME)

all: clean updateappconfig updatefavicon updatewebcontent esp8266 esp32

updatefavicon:
	python3 utils/update-favicon.py

updatewebcontent:
	python3 utils/update-webstore.py

updateappconfig:
	python3 utils/update-appconfig.py
	
esp8266 esp32:
	cp ./appconfig_$@.json ./appconfig.json 
	python3 utils/update-appconfig.py
	arduino-cli core install $@:$@
	arduino-cli compile --build-path "$(build_path)" --profile $@

	mkdir -p "$(release_path)"
	$(eval BINFILE := $(wildcard ${build_path}/*.ino.bin))

	mv "./$(build_path)/"*.ino.bin "./$(release_path)/firmware-v$(version_info)_$@_$(project_name).bin"

rp2040:
	cp ./appconfig_$@.json ./appconfig.json 
	python3 utils/update-appconfig.py
	arduino-cli core install $@:$@
	arduino-cli compile --build-path "$(build_path)" --profile $@

	mkdir -p "$(release_path)"
	$(eval BINFILE := $(wildcard ${build_path}/*.ino.uf2))

	mv "./$(build_path)/"*.ino.uf2 "./$(release_path)/firmware-v$(version_info)_$@_$(project_name).uf2"

rp2040_debug:
	arduino-cli core install rp2040:rp2040
	arduino-cli compile --build-path "$(build_path)" --profile $@

	mkdir -p "$(release_path)"
	$(eval BINFILE := $(wildcard ${build_path}/*.ino.elf))

	cp "./$(build_path)/"*.ino.elf "./$(build_path)/rp2040_debug.elf"

clean:
	mkdir -p build
	rm -fR build/*
