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
 **\file esp32wifi.c
 **
 ** Handle WiFi
 ** A detailed description is available at
 ** @link ESP32WifiGroup file description @endlink
 **
 ** History:
 ** - 2021-1-2  1.00  Manuel Schreiner
 *******************************************************************************
 */

#define __ESP32WIFI_C__

/**
 *******************************************************************************
 ** Include files
 *******************************************************************************
 */


#include "esp32wifi.h"
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>

/**
 *******************************************************************************
 ** Local pre-processor symbols/macros ('#define') 
 *******************************************************************************
 */

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10          /* Time ESP32 will go to sleep (in seconds)       */
#define TIME_TO_WAKE   10          /* Time between sleep waiting for connection      */
#define TIMEOUT_SLEEP  60          /* Seconds after last keepout before sleep        */

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

static uint32_t millisOld = 0;
static volatile uint32_t u32Counter = 1;
static char* _ssid;
static char* _password;
static en_esp32_wifi_mode_t _mode = enESP32WifiModeSoftAP;

/**
 *******************************************************************************
 ** Local function prototypes ('static') 
 *******************************************************************************
 */


static void ConnectStation(void);
static void ConnectSoftAP(void);

/**
 *******************************************************************************
 ** Function implementation - global ('extern') and local ('static') 
 *******************************************************************************
 */

/**
 * Init WiFi
 * 
 * \param mode      Station or AP mode: enESP32WifiModeSoftAP or enESP32WifiModeStation
 * 
 * \param ssid      WiFi SSID
 * 
 * \param password  WiFi Password
 * 
 */
void Esp32Wifi_Init(en_esp32_wifi_mode_t mode, const char* ssid, const char* password)
{
    _mode = mode;
    _ssid = (char*)ssid;
    _password = (char*)password;
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // ESP32 wakes up every 5 seconds
    Esp32Wifi_Connect();
}

/*
 * Connect Station Mode
 */
static void ConnectStation(void)
{
  if (WiFi.status() != WL_CONNECTED)
  {
      WiFi.mode(WIFI_STA);
      WiFi.begin(_ssid, _password);
      //Serial.println("WiFi initiated...");
    
      // Wait for connection
      uint32_t u32Tries = 1;
      while (WiFi.status() != WL_CONNECTED) 
      {
          delay(500);
          //Serial.print(".");
          u32Tries++;
          if ((u32Tries % 10) == 0)
          {
            WiFi.mode(WIFI_STA);
            WiFi.begin(_ssid, _password);
            //Serial.println("WiFi initiated...");
          }
      }
  }
}

/*
 * Connect/Create AP
 */
static void ConnectSoftAP(void)
{
    static bool bConnected = false;
    if (!bConnected)
    {
      bConnected = true;
      WiFi.softAP(_ssid, _password);
      IPAddress myIP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(myIP);
    }
}

/*
 * Connect to AP / create an AP
 */
void Esp32Wifi_Connect(void)
{
    switch(_mode)
    {
      case enESP32WifiModeSoftAP:
          ConnectSoftAP();
          break;
      case enESP32WifiModeStation:
          ConnectStation();
          break;
      default:
          break;
    }
    
}

/*
 * Update in loop, so ESP32 can be put to sleep mode in station mode
 */
void Esp32Wifi_Update(void)
{
    if (millis() != millisOld)
    {
        u32Counter++;
        millisOld = millis();
    }
    if (u32Counter > (TIMEOUT_SLEEP*1000))
    {
        //Serial.println("sleep");
        //Serial.flush(); 
        if (_mode == enESP32WifiModeStation) 
        {
          esp_light_sleep_start();
        }
        //Serial.println("wake");
        Esp32Wifi_Connect();
        u32Counter = ((TIMEOUT_SLEEP-TIME_TO_WAKE)*1000);
    }
}

/*
 * Execute to keep WiFi alive
 */
void Esp32Wifi_KeepAlive(void)
{
    u32Counter = 0;
}

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
