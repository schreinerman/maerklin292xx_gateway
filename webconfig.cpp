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
 **\file webconfig.c
 **
 ** Web Configuration
 ** A detailed description is available at
 ** @link WebConfigGroup file description @endlink
 **
 ** History:
 ** - 2021-2-3  1.00  Manuel Schreiner
 *******************************************************************************
 */

#define __WEBCONFIG_C__

/**
 *******************************************************************************
 ** Include files
 *******************************************************************************
 */


#include "stdint.h"
#include "webconfig.h"
#include "appconfig.h"
#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WebServer.h>
#else
  #include <WebServer.h>
#endif

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

/**
 *******************************************************************************
 ** Local function prototypes ('static') 
 *******************************************************************************
 */
static stc_webconfig_handle_t* pstcWebConfig;

#if defined(ARDUINO_ARCH_ESP8266)
static ESP8266WebServer* _pServer;
#else
static WebServer* _pServer;
#endif

/**
 *******************************************************************************
 ** Function implementation - global ('extern') and local ('static') 
 *******************************************************************************
 */

/*************************************
 * ToString Function for type in a buffer at a special position
 * 
 * \param type variable type
 * 
 * \param pu8Data buffer
 * 
 * \param ppos pointer to position of buffer (will be automatically incremented
 * 
 * \return String
 *************************************
 */
static String typeToString(en_webconfig_type_t type, uint8_t* pu8Data, int* ppos)
{
    uint8_t tmp[130];
    memset(tmp,0,130);
    switch(type)
    {
        case enWebConfigTypeStringLen32:
           tmp[32] = 0;
           memcpy(tmp,&pu8Data[*ppos],32);
           *ppos = *ppos + 32;
           return String((char*)tmp);
        case enWebConfigTypeStringLen64:
           tmp[64] = 0;
           memcpy(tmp,&pu8Data[*ppos],64);
           *ppos = *ppos + 64;
           return String((char*)tmp);
        case enWebConfigTypeStringLen128:
          tmp[128] = 0;
          memcpy(tmp,&pu8Data[*ppos],128);
          *ppos = *ppos + 128;
          return String((char*)tmp);
        case enWebConfigTypeUInt8:
          memcpy(tmp,&pu8Data[*ppos],1);
          *ppos = *ppos + 1;
          return String(*((uint8_t*)tmp));
        case enWebConfigTypeInt8:
          memcpy(tmp,&pu8Data[*ppos],1);
          *ppos = *ppos + 1;
          return String(*((int8_t*)tmp));
        case enWebConfigTypeUInt16:
          memcpy(tmp,&pu8Data[*ppos],2);
          *ppos = *ppos + 2;
          return String(*((uint16_t*)tmp));
        case enWebConfigTypeInt16:
          memcpy(tmp,&pu8Data[*ppos],2);
          *ppos = *ppos + 2;
          return String(*((int16_t*)tmp));
        case enWebConfigTypeUInt32:
          memcpy(tmp,&pu8Data[*ppos],4);
          *ppos = *ppos + 4;
          return String(*((uint32_t*)tmp));
        case enWebConfigTypeInt32:
          memcpy(tmp,&pu8Data[*ppos],4);
          *ppos = *ppos + 4;
          return String(*((int32_t*)tmp));
        case enWebConfigTypeBool:
          memcpy(tmp,&pu8Data[*ppos],1);
          *ppos = *ppos + 1;
          if (tmp[0] == 0) return String(false);
          return String(true);
        default:
          return String("");
    }
}

/*************************************
 * Set a value in a configuration buffer
 * 
 * \param index variable index
 * 
 * \param value new value
 * 
 ************************************* 
 */
static void setValue(int index,char* value)
{
  int dataPos = 0;
  int tmp;

  for (int i = 0; i < index;i++)
  {
    typeToString(pstcWebConfig->astcData[i].type, pstcWebConfig->pu8Data, &dataPos);
  }
  switch(pstcWebConfig->astcData[index].type)
  {
    case enWebConfigTypeStringLen32:
       strncpy((char*)&pstcWebConfig->pu8Data[dataPos],value,32);
       return;
    case enWebConfigTypeStringLen64:
       strncpy((char*)&pstcWebConfig->pu8Data[dataPos],value,64);
       return;
    case enWebConfigTypeStringLen128:
       strncpy((char*)&pstcWebConfig->pu8Data[dataPos],value,128);
       return;
    case enWebConfigTypeUInt8:
       sscanf(value,"%d",&tmp);
       *((uint8_t*)&pstcWebConfig->pu8Data[dataPos]) = tmp;
       return;
    case enWebConfigTypeInt8:
       sscanf(value,"%d",&tmp);
       *((int8_t*)&pstcWebConfig->pu8Data[dataPos]) = tmp;
       return;
    case enWebConfigTypeUInt16:
       sscanf(value,"%d",&tmp);
       *((uint16_t*)&pstcWebConfig->pu8Data[dataPos]) = tmp;
       return;
    case enWebConfigTypeInt16:
       sscanf(value,"%d",&tmp);
       *((int16_t*)&pstcWebConfig->pu8Data[dataPos]) = tmp;
       return;
    case enWebConfigTypeUInt32:
       sscanf(value,"%d",&tmp);
       *((uint32_t*)&pstcWebConfig->pu8Data[dataPos]) = tmp;
       return;
    case enWebConfigTypeInt32:
       sscanf(value,"%d",&tmp);
       *((int32_t*)&pstcWebConfig->pu8Data[dataPos]) = tmp;
       return;
  }
}

/*************************************
 * Handle /config/
 * 
 ************************************* 
 */
static void handleConfig() {
  int dataPos = 0;
  int i;
  String postForms = 
  "<html>"
  "<head>"
  "<title>App Configuration</title>"
  "</head>"
  "<body>"
  "<form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">"
  "<table>";
  
  for(i = 0;i < pstcWebConfig->ItemCount;i++)
  {
    postForms += "<tr><td>" + String(pstcWebConfig->astcData[i].description) + "</td><td><input type=\"text\" ";
    postForms += "value=\"" + typeToString(pstcWebConfig->astcData[i].type,pstcWebConfig->pu8Data,&dataPos) + "\" ";
    postForms += "name=\"" + String(pstcWebConfig->astcData[i].name) + "\"></td></tr>";
  }
  postForms += "<tr><td>&nbsp;</td><td><input type=\"submit\" value=\"Submit\"></td></tr>"
  "</table>"
  "</form>"
  "</body>"
  "</html>";
  _pServer->send(200,"text/html",postForms);
}

/*************************************
 * Handle /postform/
 * 
 ************************************* 
 */
void handleForm() {
  int i;
  if (_pServer->method() != HTTP_POST) {
    _pServer->send(405, "text/plain", "Method Not Allowed");
  } else {
    for (i = 0; i < _pServer->args(); i++) {
      for(int confIndex = 0;confIndex < pstcWebConfig->ItemCount;confIndex++)
      {
        if (strcmp((char*)_pServer->argName(i).c_str(),pstcWebConfig->astcData[confIndex].name) == 0)
        {
          setValue(confIndex,(char*)_pServer->arg(i).c_str());
        }
      }
    }

    AppConfig_Write();
    _pServer->sendHeader("Location", "/config/",true);
    _pServer->send(302, "text/plane",""); 
    for(i = 0;i < 10;i++)
    {
      _pServer->handleClient();
      delay(500);
    }
    ESP.restart();
  }
}

/*************************************
 * Initiate
 * 
 * \param pWebServerHandle Webserver handle
 * 
 * \param pstcHandle Web configuration handle
 * 
 ************************************* 
 */
#if defined(ARDUINO_ARCH_ESP8266)
void WebConfig_Init(ESP8266WebServer* pWebServerHandle, stc_webconfig_handle_t* pstcHandle)
#else
void WebConfig_Init(WebServer* pWebServerHandle, stc_webconfig_handle_t* pstcHandle)
#endif
{
  pstcWebConfig = pstcHandle;
  _pServer = pWebServerHandle;
  _pServer->on("/config/",handleConfig);
  _pServer->on("/config",handleConfig);
  _pServer->on("/postform/", handleForm);
}

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
