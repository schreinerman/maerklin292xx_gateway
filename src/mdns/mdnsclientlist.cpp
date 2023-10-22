/**
 *******************************************************************************
 ** Created by Manuel Schreiner
 **
 ** Copyright © 2023 Manuel Schreiner. All rights reserved.
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
 **\file mdnsclientlist.cpp
 **
 ** Listing other MDNS Clients
 ** A detailed description is available at
 ** @link MdnsClientListGroup file description @endlink
 **
 ** History:
 ** - 2023-10-22  1.00  Manuel Schreiner
 *******************************************************************************
 */

#define __MDNSCLIENTLIST_CPP__

/**
 *******************************************************************************
 ** Include files
 *******************************************************************************
 */

#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266mDNS.h>
#elif defined(ARDUINO_ARCH_ESP32)
  #include <ESPmDNS.h>
#elif defined(ARDUINO_ARCH_RP2040)
  #include <LEAmDNS.h>
#else
#error Not supported architecture
#endif

#include <string.h> //required also for memset, memcpy, etc.
#include <stdint.h>
#include <stdbool.h>
#include "mdnsclientlist.h"

/**
 *******************************************************************************
 ** Local pre-processor symbols/macros ('#define') 
 *******************************************************************************
 */

#define MAX_REMOTE_STATIONS 10
#define UPDATE_INTERVAL     (60 * 1000)

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

static stc_mdnsclientlist_item_t astcRemoteStations[MAX_REMOTE_STATIONS];
static uint32_t millisOld = 0;
const char* pstrCurrentService = NULL;

/**
 *******************************************************************************
 ** Local function prototypes ('static') 
 *******************************************************************************
 */

static void updateList(void);
static char* getChipID(void);

/**
 *******************************************************************************
 ** Function implementation - global ('extern') and local ('static') 
 *******************************************************************************
 */




static void updateList(void)
{
    Serial.println("Sending mDNS query");
    int n = MDNS.queryService(pstrCurrentService, "tcp");  // Send out query for esp tcp services
    Serial.println("mDNS query done");
    if (n > 0) {
        Serial.print(n);
        Serial.println(" service(s) found");
        if (n > MAX_REMOTE_STATIONS)
        {
            n = MAX_REMOTE_STATIONS;
        }
        memset(&astcRemoteStations[0],0,sizeof(astcRemoteStations));
        for (int i = 0; i < n; ++i) {
            strncpy(astcRemoteStations[i].ipAddress,MDNS.IP(i).toString().c_str(),18);
            Serial.println(MDNS.IP(i));
        }
    }
    Serial.println("done...");
}

void MdnsClientList_Init(const char* pstrService)
{
    pstrCurrentService = pstrService;
    updateList();
}

void MdnsClientList_Update(void)
{
    uint32_t u32Diff = millis();
    if (u32Diff < millisOld)
    {
       u32Diff = 0xFFFFFFFFul - millisOld + u32Diff;
    } else
    {
       u32Diff = u32Diff - millisOld;
    }
    if (u32Diff > UPDATE_INTERVAL)
    {
        millisOld = millis();
        updateList();
    }
}


/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */

