
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
 ** ESP32 WiFi to IR gateway for Märklin 29210
 **
 ** Set WiFi ssidAp, passwordAp for the Accesspoint to be generated
 ** Default SSID: Maerklin292xxGateway, Password: Maerklin292xxGateway
 **
 ** Set ssidStation and passwordStation to log into your local Wifi.
 ** If the local wifi can't be reached, an access point will be created
 **   
 ** History:
 ** - 2021-1-2  1.00  Manuel Schreiner
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** Include files
 *******************************************************************************
 */
#define USE_ESP8266 1

#include <Arduino.h>
#include <IRremoteESP8266.h>

#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif
#include <WiFiClient.h>

#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
#else
  #include <WebServer.h>
  #include <ESPmDNS.h>
#endif


#include "esp32wifi.h"
#include "maerklin292xxir.h"
#include "irgatewaywebserver.h"
#include "appconfig.h"

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
#else
static WebServer server(80);
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
  

  AppConfig_Init(&server);
  
  Maerklin292xxIr_Init();

  //initiate WIFI
  Esp32Wifi_DualModeInit((char*)AppConfig_GetStaSsid(),(char*)AppConfig_GetStaPassword(),ssidAp,passwordAp);
                                                                  
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  #if defined(ARDUINO_ARCH_ESP8266)
      WiFi.hostname(hostName);
  #else
     WiFi.setHostname(hostName);
  #endif
  

  if (MDNS.begin(hostName)) {
    Serial.println("MDNS responder started");
  }

  IrGatewayWebServer_Init(&server,enIrChannelAddress);

  MDNS.addService("http", "tcp", 80);
}



void loop() {
  // put your main code here, to run repeatedly:
  IrGatewayWebServer_Update();
  Esp32Wifi_Update();
  IrGatewayWebServer_Update();
  Maerklin292xxIr_Update();
}
