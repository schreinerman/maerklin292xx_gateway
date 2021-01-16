
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

#include <Arduino.h>
#include <IRremoteESP8266.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "esp32wifi.h"
#include "maerklin292xxir.h"
#include "irgatewaywebserver.h"

/**
 *******************************************************************************
 ** Local pre-processor symbols/macros ('#define') 
 *******************************************************************************
 */

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
const char *ssidStation = "MyLocalWifi";
const char *passwordStation = "MyLocalWifiPassword";
const en_maerklin_292xx_ir_address_t enIrChannelAddress = enMaerklin292xxIrAddressC;

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

  Maerklin292xxIr_Init();

  //initiate WIFI
  Esp32Wifi_DualModeInit(ssidStation,passwordStation,ssidAp,passwordAp);
                                                                  
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("maerklin292xx_gateway")) {
    Serial.println("MDNS responder started");
  }

  IrGatewayWebServer_Init(enIrChannelAddress);
}



void loop() {
  // put your main code here, to run repeatedly:
  IrGatewayWebServer_Update();
  Esp32Wifi_Update();
  IrGatewayWebServer_Update();
  Maerklin292xxIr_Update();
}
