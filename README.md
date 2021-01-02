# maerklin292xx_gateway
ESP32 WiFi to IR gateway for Märklin 29210

Main Module 
-----------
is the Arduino File maerklin29210_gateway.ino

IrGatewayWebServer module
--------------------------
is providing the webservice and creates following enry points:
- http://IPADDRESS/ Webpage
- http://IPADDRESS/cmd/speed/N N =-3…0…3, Speed in 3 steps for backward, forward, 0 = stop
- http://IPADDRESS/cmd/light toggle light on or off
- http://IPADDRESS/cmd/sound/horn play horn sound
- http://IPADDRESS/cmd/sound/motor play motor sound
- http://IPADDRESS/cmd/sound/coupler play coupler sound

HtmlFs module
-------------
contains the web content and is automatically generated via create_web_store.py.
To update the webpage change files in the html folder and navigate to the root folder of this repository and execute 
````
python create_web_store.py
````

ESP32Wifi
---------
handles setup of Station or SoftAP mode and going into sleep mode for station mode, so power can be saved

See more information at: http://blog.io-expert.com/modernisiert-marklin-kinderspielzeug
