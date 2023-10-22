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
 **\file appconfig.c
 **
 ** App Configuration
 ** A detailed description is available at
 ** @link AppConfigGroup file description @endlink
 **
 ** History:
 ** - 2021-2-3  1.00  Manuel Schreiner
 *******************************************************************************
 */

#define __APPCONFIG_C__

/**
 *******************************************************************************
 ** Include files
 *******************************************************************************
 */

#include <Arduino.h>
#include "stdint.h"
#include "appconfig.h"
#include "./wifimcu/webconfig.h"

#if defined(ARDUINO_ARCH_ESP8266)
#include <EEPROM.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include "FS.h"
#include "SPIFFS.h"
#define USE_SPIFFS
#elif defined(ARDUINO_ARCH_RP2040)
  #include <EEPROM.h>
#else
#error Not supported architecture
#endif

/**
 *******************************************************************************
 ** Local pre-processor symbols/macros ('#define') 
 *******************************************************************************
 */

#pragma GCC optimize ("-O3")

#define FORMAT_SPIFFS_IF_FAILED true


/**
 *******************************************************************************
 ** Global variable definitions (declared in header file with 'extern') 
 *******************************************************************************
 */


stc_appconfig_t stcAppConfig = {
  {INITIAL_SSID_STATION_MODE},
  {INITIAL_PASSWORD_STATION_MODE},
  {INITIAL_WWW_NAME},
  {INITIAL_WWW_PASS},
  
  12, // GpioIr
  -32, // GpioStatus
  -32, // GpioButton

  0xCFDFAABBUL
};

stc_webconfig_description_t astcAppConfigDescription[] = {
    {enWebConfigTypeStringLen32,"ssid","Wifi-SSID"},
    {enWebConfigTypeStringLen32,"password","Wifi-PSK"},
    {enWebConfigTypeStringLen32,"wwwuser","WWW-User"},
    {enWebConfigTypeStringLen32,"wwwpass","WWW-Pass"},
    
    {enWebConfigTypeInt32,"GpioIr","GPIO IR LED"},
    {enWebConfigTypeInt32,"GpioStatus","GPIO Status LED"},
    {enWebConfigTypeInt32,"GpioButton","GPIO Button"},

};

stc_webconfig_handle_t stcWebConfig = {
  (uint8_t*)&stcAppConfig,
  (uint32_t)sizeof(stcAppConfig),
  (uint32_t)((sizeof(astcAppConfigDescription)/sizeof(astcAppConfigDescription[0]))),
  (stc_webconfig_description_t*)astcAppConfigDescription
};

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

static bool bInitDone = false;
static bool bWebServerInitDone = false;
static bool bLockWrite = false;
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

static bool InitData()
{
  static bool bInitalized = false;
  if (!bInitalized)
  {
    #if defined(USE_SPIFFS)
      if (SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
      {
        bInitalized = true;
      } else
      {
        SPIFFS.format();
        if (SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
        {
          bInitalized = true;
        }
      }
    #else
      EEPROM.begin(1024);
      bInitalized = true;
    #endif
  }
  return bInitalized;
}
static void ReadData()
{
  #if defined(USE_SPIFFS)
    if (SPIFFS.exists("/config.bin"))
    {
      File file = SPIFFS.open("/config.bin",FILE_READ);
      file.readBytes((char*)&stcAppConfig,sizeof(stcAppConfig));
      file.close();
    }
  #else
    EEPROM.get(0,stcAppConfig);
  #endif
}

static void WriteData()
{
  #if defined(USE_SPIFFS)
    File file = SPIFFS.open("/config.bin",FILE_WRITE);
    if(file){
      file.write((uint8_t*)&stcAppConfig,sizeof(stcAppConfig));
      file.close();
    }
  #else
    EEPROM.put(0,stcAppConfig);
    EEPROM.commit();
  #endif
}

/*********************************************
 * Init App Configuration
 * 
 * pWebServerHandle Webserver handle
 * 
 ********************************************* 
 */
#if defined(ARDUINO_ARCH_ESP8266)
void AppConfig_Init(ESP8266WebServer* pWebServerHandle)
#else
void AppConfig_Init(WebServer* pWebServerHandle)
#endif
{
  if (bInitDone == false)
  {
    bInitDone = true;
    memset(&stcAppConfig,0,sizeof(stcAppConfig));
    if (!InitData())
    {
      return;
    }
    ReadData();
    if (stcAppConfig.u32magic != 0xCFDFAABB)
    {
      bLockWrite = true;
      memset(&stcAppConfig,0,sizeof(stcAppConfig));
      stcAppConfig.u32magic = 0xCFDFAABB;
      AppConfig_SetStaSsid(INITIAL_SSID_STATION_MODE);
      AppConfig_SetStaPassword(INITIAL_PASSWORD_STATION_MODE);
      AppConfig_SetWwwUser(INITIAL_WWW_NAME);
      AppConfig_SetWwwPass(INITIAL_WWW_PASS);
      
      AppConfig_SetGpioIr(12);
      AppConfig_SetGpioStatus(-32);
      AppConfig_SetGpioButton(-32);

      bLockWrite = false;
      AppConfig_Write();
    }
    
  }
  if ((bWebServerInitDone != true) && (pWebServerHandle != NULL))
  {
    bWebServerInitDone = true;
    WebConfig_Init(pWebServerHandle,&stcWebConfig);
  }
}

/*********************************************
 * Write App Configuration
 * 
 ********************************************* 
 */
void AppConfig_Write(void)
{
  if (bLockWrite == false)
  {
    bLockWrite = true;
    Serial.println("Updating App Configuration...");

    WriteData();
    
    bLockWrite = false;
  }
}

/*********************************************
 * Get STA-Mode SSID
 * 
 * \return SSID as string
 ********************************************* 
 */
const char* AppConfig_GetStaSsid(void)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  return stcAppConfig.ssidStation;
}


/*********************************************
 * Set STA-Mode SSID
 * 
 * \param ssid SSID
 * 
 ********************************************* 
 */
void AppConfig_SetStaSsid(const char* ssid)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  strncpy(stcAppConfig.ssidStation,ssid,32);
  AppConfig_Write();
}

/*********************************************
 * Get STA-Mode Password
 * 
 * \return password as string
 ********************************************* 
 */
const char* AppConfig_GetStaPassword(void)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  return stcAppConfig.passwordStation;
}

/*********************************************
 * Set STA-Mode Password
 * 
 * \param pass Password
 * 
 ********************************************* 
 */
void AppConfig_SetStaPassword(const char* pass)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  strncpy(stcAppConfig.passwordStation,pass,32);
  AppConfig_Write();
}


/**********************************************
 * Get WwwUser - WWW-User
 * 
 * \return WwwUser
 **********************************************
 */
const char* AppConfig_GetWwwUser(void)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  return stcAppConfig.wwwUser;
}

/*********************************************
 * Set WwwUser - WWW-User
 * 
 * \param WwwUser WWW-User
 * 
 ********************************************* 
 */
void AppConfig_SetWwwUser(const char* WwwUser)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  strncpy(stcAppConfig.wwwUser,WwwUser,32);
  AppConfig_Write();
}
/**********************************************
 * Get WwwPass - WWW-Pass
 * 
 * \return WwwPass
 **********************************************
 */
const char* AppConfig_GetWwwPass(void)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  return stcAppConfig.wwwPass;
}

/*********************************************
 * Set WwwPass - WWW-Pass
 * 
 * \param WwwPass WWW-Pass
 * 
 ********************************************* 
 */
void AppConfig_SetWwwPass(const char* WwwPass)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  strncpy(stcAppConfig.wwwPass,WwwPass,32);
  AppConfig_Write();
}


/**********************************************
 * Get GpioIr - GPIO IR LED
 * 
 * \return GpioIr
 **********************************************
 */
int32_t AppConfig_GetGpioIr(void)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  return stcAppConfig.GpioIr;
}

/*********************************************
 * Set GpioIr - GPIO IR LED
 * 
 * \param GpioIr GPIO IR LED
 * 
 ********************************************* 
 */
void AppConfig_SetGpioIr(int32_t GpioIr)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  stcAppConfig.GpioIr = GpioIr;
  AppConfig_Write();
}
/**********************************************
 * Get GpioStatus - GPIO Status LED
 * 
 * \return GpioStatus
 **********************************************
 */
int32_t AppConfig_GetGpioStatus(void)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  return stcAppConfig.GpioStatus;
}

/*********************************************
 * Set GpioStatus - GPIO Status LED
 * 
 * \param GpioStatus GPIO Status LED
 * 
 ********************************************* 
 */
void AppConfig_SetGpioStatus(int32_t GpioStatus)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  stcAppConfig.GpioStatus = GpioStatus;
  AppConfig_Write();
}
/**********************************************
 * Get GpioButton - GPIO Button
 * 
 * \return GpioButton
 **********************************************
 */
int32_t AppConfig_GetGpioButton(void)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  return stcAppConfig.GpioButton;
}

/*********************************************
 * Set GpioButton - GPIO Button
 * 
 * \param GpioButton GPIO Button
 * 
 ********************************************* 
 */
void AppConfig_SetGpioButton(int32_t GpioButton)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  stcAppConfig.GpioButton = GpioButton;
  AppConfig_Write();
}


/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
