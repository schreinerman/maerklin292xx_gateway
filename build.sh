#!/bin/sh
case $1 in
    "prepare")
        PACKAGES="curl wget bzip2 tar ca-certificates make python3 python3-pip"
        SUDO_REQUIRED=""

        if [ "$(whoami)" != "root" ]
        then
            SUDO_REQUIRED="sudo"
        fi

        

        if [ "$(command -v "apt-get")" ]
        then
            $SUDO_REQUIRED apt-get update
            $SUDO_REQUIRED apt-get upgrade
            PACKMAN_INSTALL="$SUDO_REQUIRED apt-get install -yq "
        elif [ "$(command -v "apk")" ]
        then
            PACKMAN_INSTALL="$SUDO_REQUIRED apk add "
        elif [ "$(command -v "brew")" ]
        then
            brew update
            PACKMAN_INSTALL="$SUDO_REQUIRED brew install "
        fi

        $PACKMAN_INSTALL $PACKAGES

        curl "https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh" -o arduino-cli-install.sh
        $SUDO_REQUIRED bash -c "BINDIR=/usr/bin sh arduino-cli-install.sh"
        mkdir ~/.arduino15
        touch ~/.arduino15/package_esp8266com_index.json
        arduino-cli config init
        arduino-cli config add board_manager.additional_urls "http://arduino.esp8266.com/stable/package_esp8266com_index.json"
        arduino-cli config add board_manager.additional_urls "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"
        arduino-cli config add board_manager.additional_urls "https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json"
        arduino-cli update
        pip3 install -r requirements.txt
        for line in  $(cat requirements.txt | xargs)
        do
            $PACKMAN_INSTALL python3-$line || true
        done
        $SUDO_REQUIRED chmod +x ./build.sh
        ;;
    "build")
        make all
        ;;

    "test")
        echo nothing to do for test...
        ;;

    *)
        echo unknown stage $1...
        ;;

esac
