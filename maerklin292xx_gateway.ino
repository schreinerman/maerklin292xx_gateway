
/**
 *******************************************************************************
 ** Created by Manuel Schreiner
 **
 ** Copyright © 2021 io-expert.com. All rights reserved.
 **
 ** 1. Redistributions of source code must retain the above copyright notice,
 **    this condition and the following disclaimer.
 **
 ** This software is provided by the copyright holder and contributors "AS IS"
 ** and any warranties related to this software are DISCLAIMED.
 ** The copyright owner or contributors be NOT LIABLE for any damages caused
 ** by use of this software.

 *******************************************************************************
 */

/**
 *******************************************************************************
 **\file maerklin29210_gateway.ino
 **
 ** ESP8266 / ESP32 WiFi to IR gateway for Märklin IR 
 **
 ** First Startup with
 ** Default SSID: Maerklin292xxGateway, Password: Maerklin292xxGateway
 ** DNS: http://maerklin-ir-gateway.local or via IP http://192.168.4.1 
 **
 ** To log into your local Wifi, change settings at http://maerklin-ir-gateway.local/config
 ** If the local wifi can't be reached, an access point Maerklin292xxGateway will be created again
 **
 ** Different IR GPIO setups can be configured as well at http://maerklin-ir-gateway.local/config
 **   
 ** History:
 ** - 2021-01-02  1.00  Manuel Schreiner  First Release
 ** - 2021-02-07  1.10  Manuel Schreiner  Configuration page was added
 ** - 2021-02-22  1.21  Manuel Schreiner  Added WiThrottle Server
 **                                       Added Firmware Update via /firmware
 **                                       Added user LED / button feature
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** Include files
 *******************************************************************************
 */

#include <Arduino.h>
#include <IRremoteESP8266.h>

#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
#else
#error Not supported architecture
#endif
#include <WiFiClient.h>

#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
#elif defined(ARDUINO_ARCH_ESP32)
  #include <WebServer.h>
  #include <ESPmDNS.h>
#else
#error Not supported architecture
#endif


#include "esp32wifi.h"
#include "maerklin292xxir.h"
#include "irgatewaywebserver.h"
#include "appconfig.h"
#include "withrottle.h"
#include "espwebupdater.h"
#include "userledbutton.h"
#include "locodatabase.h"

/**
 *******************************************************************************
 ** Local pre-processor symbols/macros ('#define') 
 *******************************************************************************
 */

#pragma GCC optimize ("-O3")

/**
 *******************************************************************************
 ** Global variable definitions (declared in header file with 'extern') 
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** Local type definitions ('typedef') 
 *******************************************************************************
 */


/**
 *******************************************************************************
 ** Local variable definitions ('static') 
 *******************************************************************************
 */
    
const char *ssidAp = "Maerklin292xxGateway";
const char *passwordAp = "Maerklin292xxGateway";
const char *hostName = "maerklin-ir-gateway";
//const char *ssidStation = "MyWifi";           --> Moved to appconfig.h, INITIAL_SSID_STATION_MODE, use http://maerklin292xx-gateway.local/config/ to configure
//const char *passwordStation = "MyPassword";   --> Moved to appconfig.h, INITIAL_PASSORD_STATION_MODE, use http://maerklin292xx-gateway.local/config/ to configure

const en_maerklin_292xx_ir_address_t enIrChannelAddress = enMaerklin292xxIrAddressC;
#if defined(ARDUINO_ARCH_ESP8266)
static ESP8266WebServer server(80);
#elif defined(ARDUINO_ARCH_ESP32)
static WebServer webServer(80);
#endif

/**
 *******************************************************************************
 ** Local function prototypes ('static') 
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** Function implementation - global ('extern') and local ('static') 
 *******************************************************************************
 */

void setup() {
  // put your setup code here, to run once:

  //intiate serial port
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Welcome to maerklin292xx gateway");

  AppConfig_Init(&webServer);

  UserLedButton_Init();
  
  Maerklin292xxIr_Init();

  LocoDatabase_Init();

  //initiate WIFI
  Esp32Wifi_DualModeInit((char*)AppConfig_GetStaSsid(),(char*)AppConfig_GetStaPassword(),ssidAp,passwordAp);
                                                                  
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  #if defined(ARDUINO_ARCH_ESP8266)
     WiFi.hostname(hostName);
  #elif defined(ARDUINO_ARCH_ESP32)
     WiFi.setHostname(hostName);
  #endif
  

  if (MDNS.begin(hostName)) {
    Serial.println("MDNS responder started");
  }
  
  EspWebUpdater_Init(&webServer);
  
  IrGatewayWebServer_Init(&webServer,enIrChannelAddress);
  
  webServer.begin();
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("irgateway","tcp",80);
  MDNS.addService("withrottle", "tcp", 2560);

  WiThrottle_Init();
}



void loop() {
  // put your main code here, to run repeatedly:
  #if defined(ARDUINO_ARCH_ESP8266)
  MDNS.update();
  #endif
  IrGatewayWebServer_Update();
  Esp32Wifi_Update();
  IrGatewayWebServer_Update();
  Maerklin292xxIr_Update();
  WiThrottle_Update();
}
