# Maerklin 292xx Gateway for Arduino IDE and ESP32
ESP32 WiFi to IR gateway for Märklin 29200 and Märklin 29210 for example using ATOM Lite with TailBat from M5Stack.

After upload to ATOM Lite, the locomotive can be controlled placed up to 10cm beside the IR sensor.

The ESP32 will initiate as SoftAP, but can be changed to Station mode, too.

Default SSID: Maerklin292xxGateway, Password: Maerklin292xxGateway

The website can be found at http://maerklin292xx_gateway.local

Requirements
------------
Board/Platform Support Packages:
- https://dl.espressif.com/dl/package_esp32_index.json
  
  find a setup guide here: https://docs.m5stack.com/#/en/arduino/arduino_development

The following library has to be installed as library and can be found in the Arduino repository: 
- IRremoteESP8266

  find a setup guide here: https://github.com/crankyoldgit/IRremoteESP8266

Main Module 
-----------
is the Arduino File maerklin29210_gateway.ino

IrGatewayWebServer module
--------------------------
is providing the webservice and creates following enry points:
- http://maerklin292xx_gateway.local/ Webpage
- http://maerklin292xx_gateway.local/cmd/speed/N N =-3…0…3, Speed in 3 steps for backward, forward, 0 = stop
- http://maerklin292xx_gateway.local/cmd/light toggle light on or off
- http://maerklin292xx_gateway.local/cmd/sound/horn play horn sound
- http://maerklin292xx_gateway.local/cmd/sound/motor play motor sound
- http://maerklin292xx_gateway.local/cmd/sound/coupler play coupler sound

additionally the channel can be defined:
- http://maerklin292xx_gateway.local/cmd/CHANNEL/CMD CHANNEL=A,B,C or D, CMD as described above.

Example:
- http://maerklin292xx_gateway.local/cmd/A/light toggled the light via channel A

HtmlFs module
-------------
contains the web content and is automatically generated via create_web_store.py.
To update the webpage change files in the html folder and navigate to the root folder of this repository and execute 
````
python create_web_store.py
````

ESP32Wifi module
----------------
handles setup of Station or SoftAP mode and going into sleep mode for station mode, so power can be saved

See more information at: http://blog.io-expert.com/modernisiert-marklin-kinderspielzeug
