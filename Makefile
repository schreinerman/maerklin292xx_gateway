
build_path:=build
release_path:=release
board_esp8266:=esp8266:esp8266:generic
board_options_esp8266:=eesz=1M64,ip=lm2f,lvl=None____, baud=115200
urls_esp8266:=http://arduino.esp8266.com/stable/package_esp8266com_index.json
board_esp32:=esp32:esp32:esp32
board_options_esp32:=PartitionScheme=min_spiffs,UploadSpeed=115200
urls_esp32:=https://dl.espressif.com/dl/package_esp32_index.json

cat := $(if $(filter $(OS),Windows_NT),type,cat)
version_info := $(shell $(cat) VERSION)

all: esp8266 esp32

esp8266 esp32:
	arduino-cli compile -b "$(board_$@)" --board-options "$(board_options_$@)" --additional-urls "$(urls_$@)" --build-path "$(build_path)" 
#	--build-property "build.extra_flags=-DAPP_VERSION=\"V$(version_info)\""

	mkdir -p "$(release_path)"
	cp "$(build_path)/maerklin292xx_gateway.ino.bin" "$(release_path)/v$(version_info)_$@_maerklin292xx_gateway.bin"

clean:
	rm -R build/*