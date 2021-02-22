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
 **\file irgatewaywebpServer->c
 **
 ** WebServer used as IR gateway
 ** A detailed description is available at
 ** @link IrGatewayWebServerGroup file description @endlink
 **
 ** History:
 ** - 2021-1-2  1.00  Manuel Schreiner
 *******************************************************************************
 */

#define __IRGATEWAYWEBSERVER_C__

/**
 *******************************************************************************
 ** Include files
 *******************************************************************************
 */

#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif
#include <WiFiClient.h>
#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WebServer.h>
  #include <uri/UriBraces.h>
#elif defined(ARDUINO_ARCH_ESP32)
  #include <WebServer.h>
#endif
#include "irgatewaywebserver.h"
#include "htmlfs.h"
#include "esp32wifi.h"
#include "maerklin292xxir.h"


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
 
//const char* update_path = "/firmware";
//const char* update_username = "admin";
//const char* update_password = "admin";

//static ESPHTTPUpdateServer httpUpdater;

#if defined(ARDUINO_ARCH_ESP8266)
static ESP8266WebServer* pServer;
#else
static WebServer* pServer;
#endif

static en_maerklin_292xx_ir_address_t enIrAddress = enMaerklin292xxIrAddressA;
/**
 *******************************************************************************
 ** Local function prototypes ('static') 
 *******************************************************************************
 */

static void processCommand(String channel, String command, String commandArg);

/**
 *******************************************************************************
 ** Function implementation - global ('extern') and local ('static') 
 *******************************************************************************
 */

static void processCommand(String channel, String command, String commandArg)
{
    Esp32Wifi_KeepAlive();
    if (channel != "")
    {
        if (channel == "A")
        {
           enIrAddress = enMaerklin292xxIrAddressA;
        } else if (channel == "B")
        {
           enIrAddress = enMaerklin292xxIrAddressB;
        } else if (channel == "C")
        {
           enIrAddress = enMaerklin292xxIrAddressC;
        } else if (channel == "D")
        {
           enIrAddress = enMaerklin292xxIrAddressD;
        } else if (channel == "G")
        {
           enIrAddress = enMaerklin292xxIrAddressG;
        } else if (channel == "H")
        {
           enIrAddress = enMaerklin292xxIrAddressH;
        } else if (channel == "I")
        {
           enIrAddress = enMaerklin292xxIrAddressI;
        } else if (channel == "J")
        {
           enIrAddress = enMaerklin292xxIrAddressJ;
        }
    }
    if (command == "sound")
    {
        if ((commandArg == "motor") || (commandArg == "3"))
        {
            Maerklin292xxIr_ToggleSoundLight(enIrAddress,enMaerklin292xxIrFuncSound1);
        } else if ((commandArg == "horn") || (commandArg == "2"))
        {
            Maerklin292xxIr_ToggleSoundLight(enIrAddress,enMaerklin292xxIrFuncSound2);
        } else if ((commandArg == "coupler") || (commandArg == "1"))
        {
            Maerklin292xxIr_ToggleSoundLight(enIrAddress,enMaerklin292xxIrFuncSound3);
        } 
    } else if (command == "speed")
    {
      int speed = commandArg.toInt();
      Maerklin292xxIr_SetSpeed(enIrAddress,speed);
    } else if (command == "forward")
    {
        Maerklin292xxIr_Send(enIrAddress,enMaerklin292xxIrFuncForward);
    } else if (command == "stop")
    {
        Maerklin292xxIr_Send(enIrAddress,enMaerklin292xxIrFuncStop);
    }  else if (command == "backward")
    {
        Maerklin292xxIr_Send(enIrAddress,enMaerklin292xxIrFuncBackward);
    } else if (command == "light")
    {
        Maerklin292xxIr_ToggleSoundLight(enIrAddress,enMaerklin292xxIrFuncLight);
    } else if (command == "keepalive")
    {
        //Serial.println("k");
    }
    pServer->send(200, "text/plain", "done");
}

/*
 * Init Webserver Service
 * 
 * \param enIrChannelAddress default IR channel
 */
#if defined(ARDUINO_ARCH_ESP8266)
  void IrGatewayWebServer_Init(ESP8266WebServer* pWebServer, en_maerklin_292xx_ir_address_t enIrChannelAddress)
#else
  void IrGatewayWebServer_Init(WebServer* pWebServer, en_maerklin_292xx_ir_address_t enIrChannelAddress)
#endif
{
  pServer = pWebServer;
  enIrAddress = enIrChannelAddress;
  

  #if defined(ARDUINO_ARCH_ESP8266)
    pServer->on(UriBraces("/cmd/{}/{}/{}"), []() {
  #else
    pServer->on("/cmd/{}/{}/{}", []() {
  #endif
    String channel = pServer->pathArg(0);
    String cmd = pServer->pathArg(1);
    String cmdArgs = pServer->pathArg(2);
    processCommand(channel,cmd,cmdArgs);
  });

  #if defined(ARDUINO_ARCH_ESP8266)
    pServer->on(UriBraces("/cmd/{}/{}"), []() {
  #else
    pServer->on("/cmd/{}/{}", []() {
  #endif
    String channel = pServer->pathArg(0);
    String cmd = pServer->pathArg(1);
    String cmdArgs = "";
    if (!((channel.charAt(0) >= 'A') && (channel.charAt(0) <= 'Z')))
    {
       channel = "";
       cmd = pServer->pathArg(0);
       cmdArgs = pServer->pathArg(1);
    }
    processCommand(channel,cmd,cmdArgs);
  });

  #if defined(ARDUINO_ARCH_ESP8266)
    pServer->on(UriBraces("/cmd/{}"), []() {
  #else
    pServer->on("/cmd/{}", []() {
  #endif
    String cmd = pServer->pathArg(0);
    Esp32Wifi_KeepAlive();
    if (cmd == "keepalive")
    {
        pServer->send(200, "text/plain", "keep alive accepted");
    } else
    {
        processCommand("",cmd,"");
    } 
  });

  HtmlFs_Init(pServer);

  //httpUpdater.setup(pServer, update_path, update_username, update_password);

  
  //Serial.println("HTTP server started");
}

/*
 * Update Webserver in loop
 */
void IrGatewayWebServer_Update(void)
{
    pServer->handleClient();
}
/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
