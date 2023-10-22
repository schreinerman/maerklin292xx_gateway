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
 **\file withrottle.c
 **
 ** Small WiThrootle Server
 ** A detailed description is available at
 ** @link WiThrottleGroup file description @endlink
 **
 ** History:
 ** - 2021-2-21  1.00  Manuel Schreiner
 *******************************************************************************
 */

#if !defined(__WITHROTTLE_H__)
#define __WITHROTTLE_H__

/* C binding of definitions if building with C++ compiler */
//#ifdef __cplusplus
//extern "C"
//{
//#endif

/**
 *******************************************************************************
 ** \defgroup WiThrottleGroup Small WiThrootle Server
 **
 ** Provided functions of WiThrottle:
 **
 **
 *******************************************************************************
 */

//@{

/**
 *******************************************************************************
** \page withrottle_module_includes Required includes in main application
** \brief Following includes are required
** @code
** #include "withrottle.h"
** @endcode
**
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** (Global) Include files
 *******************************************************************************
 */

#include <stdint.h>
#include "Arduino.h"
#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif
#include <stdarg.h>

/**
 *******************************************************************************
 ** Global pre-processor symbols/macros ('#define') 
 *******************************************************************************
 */

#define WITHROTTLE_MAX_BUFFER_PER_LOCO 64
/**
 *******************************************************************************
 ** Global type definitions ('typedef') 
 *******************************************************************************
 */

typedef struct stc_withrottle_client
{
  WiFiClient client;
  uint8_t buffer[WITHROTTLE_MAX_BUFFER_PER_LOCO];
  uint8_t bufferPos;
  
} stc_withrottle_client_t;

struct stc_withrottle_loco;


typedef void (*pfn_withrottle_loco_speed_callback_t)(struct stc_withrottle_loco* pHandle, bool bForward, uint8_t speed);
typedef bool (*pfn_withrottle_loco_function_toggle_callback_t)(struct stc_withrottle_loco* pHandle, uint8_t u8Function);

typedef struct stc_withrottle_loco
{
   bool bLongAddress;
   uint32_t u32Address;
   pfn_withrottle_loco_speed_callback_t cbSpeedUpdated;
   pfn_withrottle_loco_function_toggle_callback_t cbFunctionUpdated;
   bool bDir;
   int speed;
   uint32_t u32FunctionMask;
} stc_withrottle_loco_t;

struct stc_withrottle_loco_listitem;

typedef struct stc_withrottle_loco_listitem
{
  stc_withrottle_loco_t locoItem;
  struct stc_withrottle_loco_listitem* pNextItem;
} stc_withrottle_loco_listitem_t;

typedef void (*pfn_withrottle_trackpower_callback_t)(bool onoff);
typedef stc_withrottle_loco_listitem_t* (*pfn_withrottle_createloco_callback_t)(uint32_t u32Address, bool bLongAddress);




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


void WiThrottle_Init(void);
void WiThrottle_Disconnect(void);
void WiThrottle_Update(void);
void WiThrottle_Printf(stc_withrottle_client_t* pClient,char* format,...);
void WiThrottle_ReceivedBuffer(stc_withrottle_client_t* pClient, uint8_t* pu8Data,uint32_t u32DataLen);
void WiThrottle_ReceivedChar(stc_withrottle_client_t* pClient, uint8_t dataChar);
void WiThrottle_AddLoco(stc_withrottle_loco_listitem_t* pLocoItem);
void WiThrottle_CreateLocoCallback(pfn_withrottle_createloco_callback_t cbCreateLoco);
void WiThrottle_RegisterPowerCallback(pfn_withrottle_trackpower_callback_t cbTrackPower);


//@} // WiThrottleGroup

//#ifdef __cplusplus
//}
//#endif

#endif /* __WITHROTTLE_H__ */

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
