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
#include "webconfig.h"
#include <EEPROM.h>
 #if defined(ARDUINO_ARCH_ESP32)

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


stc_appconfig_t stcAppConfig = {
  INITIAL_SSID_STATION_MODE,
  INITIAL_PASSORD_STATION_MODE,
  INITIAL_GPIO_IR,
  0xFFFFAABB
};

stc_webconfig_description_t astcAppConfigDescription[] = {
    {enWebConfigTypeStringLen32,"ssid","SSID Wifi Router"},
    {enWebConfigTypeStringLen32,"password","Password Wifi Router"},
    {enWebConfigTypeUInt32,"gpioir","GPIO IR LED"}
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
    EEPROM.begin(512);
    EEPROM.get(0,stcAppConfig);
    bInitDone = true;
    if (stcAppConfig.u32magic != 0xCFDFAABB)
    {
      bLockWrite = true;
      memset(&stcAppConfig,0,sizeof(stcAppConfig));
      stcAppConfig.u32magic = 0xCFDFAABB;
      AppConfig_SetStaSsid(INITIAL_SSID_STATION_MODE);
      AppConfig_SetStaPassword(INITIAL_PASSORD_STATION_MODE);
      AppConfig_SetIrGpio(INITIAL_GPIO_IR);
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
    #if defined(ARDUINO_ARCH_ESP32)
      portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
      taskENTER_CRITICAL(&myMutex);
    #endif
    noInterrupts();
    EEPROM.put(0,stcAppConfig);
    EEPROM.commit();
    interrupts();
    #if defined(ARDUINO_ARCH_ESP32)
      taskEXIT_CRITICAL(&myMutex);
    #endif
    bLockWrite = false;
  }
}

/*********************************************
 * Get STA-Mode SSID
 * 
 * \return SSID as string
 ********************************************* 
 */
char* AppConfig_GetStaSsid(void)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  return stcAppConfig.ssidStation;
}

/*********************************************
 * Get STA-Mode Password
 * 
 * \return password as string
 ********************************************* 
 */
char* AppConfig_GetStaPassword(void)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  return stcAppConfig.passwordStation;
}

/*********************************************
 * Get IR GPIO
 * 
 * \return IR GPIO Number
 ********************************************* 
 */
uint32_t AppConfig_GetIrGpio(void)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  return stcAppConfig.gpioIRLED;
}

/*********************************************
 * Set STA-Mode SSID
 * 
 * \param ssid SSID
 * 
 ********************************************* 
 */
void AppConfig_SetStaSsid(char* ssid)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  strncpy(stcAppConfig.ssidStation,ssid,32);
  AppConfig_Write();
}

/*********************************************
 * Set STA-Mode Password
 * 
 * \param pass Password
 * 
 ********************************************* 
 */
void AppConfig_SetStaPassword(char* pass)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  strncpy(stcAppConfig.passwordStation,pass,32);
  AppConfig_Write();
}

/*********************************************
 * Set IR GPIO
 * 
 * \param u32Gpio IR GPIO Number
 * 
 ********************************************* 
 */
void AppConfig_SetIrGpio(uint32_t u32Gpio)
{
  if (bInitDone == false)
  {
    AppConfig_Init(NULL);
  }
  stcAppConfig.gpioIRLED = u32Gpio;
  AppConfig_Write();
}

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
