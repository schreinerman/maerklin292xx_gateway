# Maerklin 292xx Gateway for Arduino IDE and ESP32
ESP32 WiFi to IR gateway for Märklin IR model trains for kids for example using ATOM Lite with TailBat from M5Stack.

Currently supported addresses A,B,C,D,G,H

See following list as reference (without warranty or guaranty)

| Name                                                  | Adresse       | Akku          | URL                                                           |
|-------------------------------------------------------|---------------|---------------|---------------------------------------------------------------|
| Märklin 29100	Nahverkehrszug LINT                     | G-H           | USB/LiIon     | https://www.maerklin.de/de/produkte/details/article/29100
| Märklin 29200	ICE                                     | A-B           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29200
| Märklin 29201	TGV                                     | A-B           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29201
| Märklin 29202	Belgischer Hochgeschwindigkeitszug      | A-B           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29202
| Märklin 29203	ICN                                     | C-D           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29203
| Märklin 29204 Amerikanischer Hochgeschwindigkeitszug  | C-D           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29204
| Märklin 29206 Mauszug                                 | C-D           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29206
| Märklin 29207 Ferien-Express                          | A-B           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29207
| Märklin 29208 Eurostar                                | A-B           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29208
| Märklin 29209 Regional Express                        | C-D           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29209
| Märklin 29210	Güterzug                                | C-D           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29200
| Märklin 29212 TGV Duplex                              | C-D           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29212
| Märklin 29300 ICE                                     | A-B           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29300
| Märklin 29302 Intercity                               | C-D           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29302 
| Märklin 29303 ICN                                     | C-D           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29303
| Märklin 29304 TGV Lyria                               | A-B           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29304
| ~~Märklin 29306 TGV~~                                 | I-J           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29306
| Märklin 29307 Airport Express - Hochbahn              | G-H           | USB/LiIon     | https://www.maerklin.de/de/produkte/details/article/29307
| ~~Märklin 29308 Landwirtschaft~~                      | I-J           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29308
| Märklin 29309 Güterzug                                | G-H           | USB/LiIon     | https://www.maerklin.de/de/produkte/details/article/29309
| ~~Märklin 29330 ICE 3~~                               | I-J           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29330
| ~~Märklin 29334 Italienischer Schnellzug~~            | M-N           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29334
| ~~Märklin 29335 Schweizer Schnellzug~~                | M-N           | 4x AA         | https://www.maerklin.de/de/produkte/details/article/29335
| Märklin 36100 Nahverkehrszug LINT                     | G-H           | USB/LiIon     | https://www.maerklin.de/de/produkte/details/article/36100
| Märklin 36101 Diesellokomotive BR 212                 | G-H           | USB/LiIon     | https://www.maerklin.de/de/produkte/details/article/36100


After upload to ATOM Lite, the locomotive can be controlled placed up to 10cm beside the IR sensor. An external IR transmitter diode can be used by chaning the GPIO channel in maerklin292xxir.h at ```#define MAERKLIN292XXIR_IR_PIN 12``` from 12 to your choice. For example chaning to 25 makes it possible to use some external IR transmitters for having a wide range conection at the bottom connector. Best is to use GND, 5V for the power supply of the IR transmitter. For example using: RM-024 LDTR


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
is the Arduino File maerklin292xx_gateway.ino

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
