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

#if !defined(__APPCONFIG_H__)
#define __APPCONFIG_H__

/* C binding of definitions if building with C++ compiler */
//#ifdef __cplusplus
//extern "C"
//{
//#endif

/**
 *******************************************************************************
 ** \defgroup AppConfigGroup App Configuration
 **
 ** Provided functions of AppConfig:
 **
 **
 *******************************************************************************
 */

//@{

/**
 *******************************************************************************
** \page appconfig_module_includes Required includes in main application
** \brief Following includes are required
** @code
** #include "appconfig.h"
** @endcode
**
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** (Global) Include files
 *******************************************************************************
 */

#include "stdint.h"
#include "webconfig.h"

/**
 *******************************************************************************
 ** Global pre-processor symbols/macros ('#define') 
 *******************************************************************************
 */

#define APP_VERSION "V1.2.1"
#define INITIAL_SSID_STATION_MODE "MyWifi"
#define INITIAL_PASSORD_STATION_MODE "MyPassword"
#if defined(ARDUINO_ARCH_ESP8266)
#define INITIAL_GPIO_IR 4
#elif defined(ARDUINO_ARCH_ESP32)
#define INITIAL_GPIO_IR 12
#else
#error Not supported architecture
#endif

/**
 *******************************************************************************
 ** Global type definitions ('typedef') 
 *******************************************************************************
 */

typedef struct stc_appconfig
{
  char ssidStation[32];
  char passwordStation[32];
  uint32_t gpioIRLED;
  uint32_t gpioStatusLED;
  int32_t gpioUserButton;
  int32_t u32magic;
} stc_appconfig_t;

/**
 *******************************************************************************
 ** Global variable declarations ('extern', definition in C source)
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** Global function prototypes ('extern', definition in C source) 
 *******************************************************************************
 */

#if defined(ARDUINO_ARCH_ESP8266)
void AppConfig_Init(ESP8266WebServer* pWebServerHandle);
#else
void AppConfig_Init(WebServer* pWebServerHandle);
#endif
void AppConfig_Write(void);
char* AppConfig_GetStaSsid(void);
char* AppConfig_GetStaPassword(void);
uint32_t AppConfig_GetIrGpio(void);
int AppConfig_GetStatusLedGpio(void);
int AppConfig_GetUserButtonGpio(void);
void AppConfig_SetStaSsid(char* ssid);
void AppConfig_SetStaPassword(char* pass);
void AppConfig_SetIrGpio(uint32_t u32Gpio);
void AppConfig_SetStatusLedGpio(int u32Gpio);
void AppConfig_SetUserButtonGpio(int u32Gpio);

//@} // AppConfigGroup

//#ifdef __cplusplus
//}
//#endif

#endif /* __APPCONFIG_H__ */

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
