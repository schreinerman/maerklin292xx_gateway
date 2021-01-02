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
 **\file irgatewaywebserver.c
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

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "irgatewaywebserver.h"
#include "htmlfs.h"
#include "esp32wifi.h"
#include "maerklin292xxir.h"


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
static WebServer server(80);

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

/*
 * Init Webserver Service
 */
void IrGatewayWebServer_Init(void)
{
  HtmlFs_Init(&server);

  server.on("/cmd/{}/{}", []() {
    String cmd = server.pathArg(0);
    String cmdArg = server.pathArg(1);
    Esp32Wifi_KeepAlive();
    if (cmd == "sound")
    {
        if ((cmdArg == "motor") || (cmdArg == "3"))
        {
            Maerklin292xxIr_Send(enMaerklin292xxIrAddressC,enMaerklin292xxIrFuncSound1);
        } else if ((cmdArg == "horn") || (cmdArg == "2"))
        {
            Maerklin292xxIr_Send(enMaerklin292xxIrAddressC,enMaerklin292xxIrFuncSound2);
        } else if ((cmdArg == "coupler") || (cmdArg == "1"))
        {
            Maerklin292xxIr_Send(enMaerklin292xxIrAddressC,enMaerklin292xxIrFuncSound3);
        } 
    } else if (cmd == "speed")
    {
      int speed = cmdArg.toInt();
      Maerklin292xxIr_Send(enMaerklin292xxIrAddressC,enMaerklin292xxIrFuncStop);
      if (speed > 0)
      {
         while(speed != 0)
         {
            speed--;
            Maerklin292xxIr_Send(enMaerklin292xxIrAddressC,enMaerklin292xxIrFuncForward);
         }
      } else if (speed < 0)
      {
         while(speed != 0)
         {
            speed++;
            Maerklin292xxIr_Send(enMaerklin292xxIrAddressC,enMaerklin292xxIrFuncBackward);
         }
      } 
    }
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/cmd/{}", []() {
    String cmd = server.pathArg(0);
    Esp32Wifi_KeepAlive();
    if (cmd == "forward")
    {
        Maerklin292xxIr_Send(enMaerklin292xxIrAddressC,enMaerklin292xxIrFuncForward);
    } else if (cmd == "stop")
    {
        Maerklin292xxIr_Send(enMaerklin292xxIrAddressC,enMaerklin292xxIrFuncStop);
    }  else if (cmd == "backward")
    {
        Maerklin292xxIr_Send(enMaerklin292xxIrAddressC,enMaerklin292xxIrFuncBackward);
    } else if (cmd == "light")
    {
        Maerklin292xxIr_Send(enMaerklin292xxIrAddressC,enMaerklin292xxIrFuncLight);
    } else if (cmd == "keepalive")
    {
        //Serial.println("k");
    }
    server.send(200, "text/plain", "this works as well");
  });
  
  server.begin();
  //Serial.println("HTTP server started");
}

/*
 * Update Webserver in loop
 */
void IrGatewayWebServer_Update(void)
{
    server.handleClient();
}
/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
