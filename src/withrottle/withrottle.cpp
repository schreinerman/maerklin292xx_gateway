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

#define __WITHROTTLE_C__

/**
 *******************************************************************************
 ** Include files
 *******************************************************************************
 */


#include "Arduino.h"

#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
#elif defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#else
#error Not supported architecture
#endif
#include "withrottle.h"
#include <stdarg.h>
#include "../wifimcu/wifimcuctrl.h"

/**
 *******************************************************************************
 ** Local pre-processor symbols/macros ('#define') 
 *******************************************************************************
 */

#define MAX_CLIENTS 3

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

static WiFiServer server(2560);
static stc_withrottle_client_t serverClients[MAX_CLIENTS];
static bool trackPower = false;
static pfn_withrottle_trackpower_callback_t _cbTrackPower = NULL;
static pfn_withrottle_createloco_callback_t _cbCreateLoco = NULL;
static stc_withrottle_loco_listitem_t* pLocoList = NULL;
static bool debugMode = false;

/**
 *******************************************************************************
 ** Local function prototypes ('static') 
 *******************************************************************************
 */

static void send(stc_withrottle_client_t* pClient, uint8_t* data,uint32_t u32Len);
static void sendString(stc_withrottle_client_t* pClient, char* str);
static void decode_multithrottle(stc_withrottle_client_t* pClient, uint8_t* cmd);
static int getInt(uint8_t * cmd);
static int getLocoId(uint8_t * cmd);
static stc_withrottle_loco_listitem* getLoco(int locoid);
static void decode_loco_action(stc_withrottle_client_t* pClient, stc_withrottle_loco_t* pLoco, uint8_t* aval, char throttleChar, int cab);

/**
 *******************************************************************************
 ** Function implementation - global ('extern') and local ('static') 
 *******************************************************************************
 */

/*********************************************
 * Send data to a connected client
 * 
 * pClient Client handle
 *
 * data    Data buffer
 *
 * u32Len  Data length
 * 
 ********************************************* 
 */
static void send(stc_withrottle_client_t* pClient, uint8_t* data,uint32_t u32Len)
{
  pClient->client.write(data, u32Len);
}

/*********************************************
 * Send string to a connected client
 * 
 * pClient Client handle
 *
 * str String
 * 
 ********************************************* 
 */
static void sendString(stc_withrottle_client_t* pClient, char* str)
{
  pClient->client.write(str, strlen(str));
}

/*********************************************
 * Get integer value from a command value
 * 
 * cmd     current pointer where the number is expected
 * 
 * \return return number
 * 
 ********************************************* 
 */
static int getInt(uint8_t * cmd) {
    int i=0;
    while (cmd[0]>='0' && cmd[0]<='9') {
      i=i*10 + (cmd[0]-'0');
      cmd++;
    }
    return i;    
}

/*********************************************
 * Get loco id from a command value
 * 
 * cmd     current pointer where the loco id is expected
 * 
 * \return loco id / address
 * 
 ********************************************* 
 */
static int getLocoId(uint8_t * cmd) {
    if (cmd[0]=='*') return -1;  // match all locos 
    if (cmd[0]!='L' && cmd[0]!='S') return 0; // should not match any locos
    return getInt(cmd+1); 
}

/*********************************************
 * Get loco structure from loco id
 * 
 * locoid     loco id
 * 
 * \return  pointer to loco structure, else NULL
 * 
 ********************************************* 
 */
static stc_withrottle_loco_listitem* getLoco(int locoid)
{
  stc_withrottle_loco_listitem_t* pCurrent = pLocoList;
  if (locoid <= 0)
  {
    return NULL;
  }
  while(pCurrent != NULL)
  {
    if (pCurrent->locoItem.u32Address == locoid)
    {
      return pCurrent;
    }
    pCurrent = pCurrent->pNextItem;
  }
  return NULL;
}

/*********************************************
 * Get L / S char (representing long/short)
 * 
 * pLoco    pointer to loco structure
 * 
 * \return  char L / S
 * 
 ********************************************* 
 */
static char LorS(stc_withrottle_loco_t* pLoco)
{
  if (pLoco == NULL)
  {
    return ' ';
  }
  if (pLoco->bLongAddress)
  {
    return 'L';
  }
  return 'S';
}

/*********************************************
 * Decode loco action
 * 
 * pClient  Pointer of connected client handle
 * 
 * pLoco    Loco handle of the current loco
 * 
 * aval     Value of the command
 * 
 * throttleChar throttle-char
 * 
 * cab      Loco ID / Address
 * 
 * 
 ********************************************* 
 */
static void decode_loco_action(stc_withrottle_client_t* pClient, stc_withrottle_loco_t* pLoco, uint8_t* aval, char throttleChar, int cab)
{
  uint8_t tmp;
  uint32_t u32Tmp;
  
  if (pLoco == NULL)
  {
    return;
  }
  switch (aval[0]) 
  {
    case 'V':
      tmp=getInt(aval+1);
      if (pLoco->cbSpeedUpdated)
      {
         pLoco->cbSpeedUpdated(pLoco,pLoco->bDir,tmp);
      }
      pLoco->speed = tmp;
      WiThrottle_Printf(pClient,"M%cA%c%d<;>V%d\n",throttleChar,LorS(pLoco),pLoco->u32Address,pLoco->speed);
      break;
    case 'R':
      if (aval[1] == '1')
      {
        tmp = 1;
      } else
      {
        tmp = 0;
      }
      if (pLoco->cbSpeedUpdated)
      {
         pLoco->cbSpeedUpdated(pLoco,tmp,pLoco->speed);
      }
      pLoco->bDir = tmp;
      WiThrottle_Printf(pClient,"M%cA%c%d<;>R%d\n",throttleChar,LorS(pLoco),pLoco->u32Address,pLoco->bDir);
      break;
    case 'X':
    case 'I':
    case 'Q':
      if (pLoco->cbSpeedUpdated)
      {
         pLoco->cbSpeedUpdated(pLoco,pLoco->bDir,0);
         pLoco->speed = 0;
      }
      WiThrottle_Printf(pClient,"M%cA%c%d<;>V%d\n",throttleChar,LorS(pLoco),pLoco->u32Address,pLoco->speed);
      break;
    case 'F':
      tmp=getInt(aval+2);
      if (aval[1] == '1')
      {
        u32Tmp = pLoco->u32FunctionMask ^ (1 << tmp);
        if (pLoco->cbFunctionUpdated)
        {
           pLoco->cbFunctionUpdated(pLoco,tmp);
        }
        pLoco->u32FunctionMask ^= (1 << tmp);
      }
      if (u32Tmp & (1 << tmp))
      {
        WiThrottle_Printf(pClient,"M%cA%c%d<;>F%d%d\n",throttleChar,LorS(pLoco),pLoco->u32Address,1,tmp);
      } 
      if ((pLoco->u32FunctionMask & (1 << tmp)) == 0)
      {
        WiThrottle_Printf(pClient,"M%cA%c%d<;>F%d%d\n",throttleChar,LorS(pLoco),pLoco->u32Address,0,tmp);
      }
      break;
    case 'q':
      if (aval[1]=='V')  //qV
      {  
        WiThrottle_Printf(pClient,"M%cA%c%d<;>V%d\n",throttleChar,LorS(pLoco),pLoco->u32Address,pLoco->speed);
      } else if (aval[1]=='R')  //qR
      {  
        WiThrottle_Printf(pClient,"M%cA%c%d<;>R%d\n",throttleChar,LorS(pLoco),pLoco->u32Address,pLoco->bDir);
      }
      break;
  }
}

/*********************************************
 * Decode multi throttle
 * 
 * pClient  Pointer of connected client handle
 * 
 * cmd      Command
 * 
 * 
 ********************************************* 
 */
static void decode_multithrottle(stc_withrottle_client_t* pClient, uint8_t* cmd)
{
  char throttleChar=cmd[1];
  int locoid=getLocoId(cmd+3); // -1 for *
  byte * aval=cmd;
  stc_withrottle_loco_listitem* pHandle = getLoco(locoid);
  
  while(*aval !=';' && *aval !='\0') aval++;
  if (*aval) aval+=2;  // skip ;>

   switch(cmd[2]) {
     case '+':
       if (locoid==0) {
         sendString(pClient,"HMAddress '0' not supported!\n");
       }
       if ((_cbCreateLoco) && (pHandle == NULL))
       {
          if (cmd[0]!='L')
          {
            pHandle = _cbCreateLoco(locoid,true);
          } else if (cmd[0]!='S')
          {
            pHandle = _cbCreateLoco(locoid,false);
          }
          if (pHandle != NULL)
          {
            WiThrottle_AddLoco(pHandle);
          }
       }
       if (pHandle != NULL)
       {
         WiThrottle_Printf(pClient,"M%c+%c%d<;>\n", throttleChar, cmd[3] ,locoid);
         for(int fKey=0; fKey<28; fKey++) { 
            if (pHandle->locoItem.u32FunctionMask & (1 << fKey))
            {
              WiThrottle_Printf(pClient,"M%cA%c%d<;>F%d%d\n",throttleChar,cmd[3],locoid,1,fKey);
            } else
            {
              WiThrottle_Printf(pClient,"M%cA%c%d<;>F%d%d\n",throttleChar,cmd[3],locoid,0,fKey);
            }
         }
         WiThrottle_Printf(pClient,"M%cA%c%d<;>V%d\n", throttleChar, cmd[3], locoid, pHandle->locoItem.speed);
         WiThrottle_Printf(pClient,"M%cA%c%d<;>R%d\n", throttleChar, cmd[3], locoid, pHandle->locoItem.bDir);
         WiThrottle_Printf(pClient,"M%cA%c%d<;>s1\n", throttleChar, cmd[3], locoid); //default speed step 128
       }
       break;
     case 'A':
       if (cmd[3] == '*')
       {
          pHandle = pLocoList;
          while(pHandle != NULL)
          {
            decode_loco_action(pClient, &pHandle->locoItem, aval, throttleChar, pHandle->locoItem.u32Address);
            pHandle = pHandle->pNextItem;
          }
       } else
       {
         if (pHandle != NULL)
         {
             decode_loco_action(pClient, &pHandle->locoItem, aval, throttleChar, locoid);
         }
       }
       break;
   }
}

/*********************************************
 * Decode incoming data
 * 
 * pClient  Pointer of connected client handle
 * 
 ********************************************* 
 */
static void decode(stc_withrottle_client_t* pClient)
{
  uint8_t* cmd = (uint8_t*)pClient->buffer;
  if (debugMode)
  {
    Serial.print("New command: \"");
    Serial.print((char*)cmd);
    Serial.println("\"");
  }
  switch(cmd[0])
  {
    case 'P': 
      if (cmd[1]=='P' && cmd[2]=='A' )
      {
        sendString(pClient,"PPA");
        if (cmd[3]=='1')
        {
          trackPower = true;
          sendString(pClient,"1");
        } else
        {
          trackPower = false;
          sendString(pClient,"0");
        }
        sendString(pClient,"\n");
      }
      break;
    case 'N':
      sendString(pClient,"*5\n");
      break;
    case 'H':
      if (cmd[1] == 'U') {
        sendString(pClient,"VN2.0\nHTIR-TRAIN\nRL0\n");
        sendString(pClient,"HtIR-TRAIN\n");
        sendString(pClient,"PPA");
        if (trackPower)
        {
          sendString(pClient,"1");
        } else
        {
          sendString(pClient,"0");
        }
        sendString(pClient,"\n");
        sendString(pClient,"*5\n");
      }      
      break;
    case 'M':
      decode_multithrottle(pClient,cmd);
  }
}

/*********************************************
 * Init Throttle
 * 
 ********************************************* 
 */
void WiThrottle_Init(void)
{
  server.begin();
  server.setNoDelay(true);
}

/*********************************************
 * Disconnect all connected throttles
 * 
 ********************************************* 
 */
void WiThrottle_Disconnect(void)
{
  static uint32_t u32LastUpdate = millis();
  if ((millis() - u32LastUpdate) > 1000)
  {
    for(int i = 0; i < MAX_CLIENTS; i++) {
      if (serverClients[i].client) serverClients[i].client.stop();
    }
    u32LastUpdate = millis();
  }
}

/*********************************************
 * Received data callback, called after data was received
 * 
 * pClient  Pointer of connected client handle
 * 
 * pu8Data  Pointer to received data
 * 
 * u32DataLen Length of received data
 * 
 ********************************************* 
 */
void WiThrottle_ReceivedBuffer(stc_withrottle_client_t* pClient, uint8_t* pu8Data,uint32_t u32DataLen)
{
  for(int i = 0;i < u32DataLen;i++)
  {
    WiThrottle_ReceivedChar(pClient,pu8Data[i]);
  }
}

/*********************************************
 * Received char callback, called after data was received
 * 
 * pClient  Pointer of connected client handle
 * 
 * dataChar  Recevied data char
 * 
 ********************************************* 
 */
void WiThrottle_ReceivedChar(stc_withrottle_client_t* pClient, uint8_t dataChar)
{
  WifiMcuCtrl_KeepAlive();
  pClient->buffer[pClient->bufferPos] = dataChar;
  pClient->bufferPos++;
  pClient->buffer[pClient->bufferPos] = 0;
  if ((dataChar == '\n') || (pClient->bufferPos > (WITHROTTLE_MAX_BUFFER_PER_LOCO-2)))
  {
    decode(pClient);
    pClient->bufferPos = 0;
    pClient->buffer[0] = 0;
  }
}


/*********************************************
 * Update WiThrottle from loop()
 * 
 ********************************************* 
 */
void WiThrottle_Update(void)
{
  int i;
  //check if there are any new clients
  if (server.hasClient()){
    for(i = 0; i < MAX_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i].client || !serverClients[i].client.connected()){
        if(serverClients[i].client) serverClients[i].client.stop();
        serverClients[i].client = server.available();
        if ((debugMode) && (!serverClients[i].client)) Serial.println("available broken");
        if (debugMode)
        {
          Serial.print("New client: ");
          Serial.print(i); Serial.print(' ');
          Serial.println(serverClients[i].client.remoteIP());
        }
        break;
      }
    }
    if (i >= MAX_CLIENTS) {
      //no free/disconnected spot so reject
      server.available().stop();
    }
  }
  //check clients for data
  for(i = 0; i < MAX_CLIENTS; i++){
    if (serverClients[i].client && serverClients[i].client.connected()){
      if(serverClients[i].client.available()){
        //get data from the telnet client and push it to the UART
        while(serverClients[i].client.available())
        {
          WiThrottle_ReceivedChar(&serverClients[i],serverClients[i].client.read());
        }
      }
    }
    else {
      if (serverClients[i].client) {
        serverClients[i].client.stop();
      }
    }
  }
}


/*********************************************
 * Printf to a connected client
 * 
 * pClient  Pointer of connected client handle
 * 
 * format   Printf format
 * 
 * ...      Parameters
 * 
 ********************************************* 
 */
void WiThrottle_Printf(stc_withrottle_client_t* pClient,char* format,...)
{
    static uint8_t au8PrintfBuffer[256];
    va_list arg;
    va_start(arg, format);
    vsnprintf((char*)au8PrintfBuffer,256,format,arg);
    sendString(pClient,(char*)au8PrintfBuffer);
    va_end(arg);
}

/*********************************************
 * Register callback, called to register a new loco
 * 
 * cbCreateLoco  Callback
 * 
 ********************************************* 
 */
void WiThrottle_RegisterCreateLocoCallback(pfn_withrottle_createloco_callback_t cbCreateLoco)
{
  _cbCreateLoco = cbCreateLoco;
}

/*********************************************
 * Register callback, called if the power state changed
 * 
 * cbTrackPower  Callback
 * 
 ********************************************* 
 */
void WiThrottle_RegisterPowerCallback(pfn_withrottle_trackpower_callback_t cbTrackPower)
{
  _cbTrackPower = cbTrackPower;
}

/*********************************************
 * Add new loco to the list of locos
 * 
 * pLocoItem  List item
 * 
 ********************************************* 
 */
void WiThrottle_AddLoco(stc_withrottle_loco_listitem_t* pLocoItem)
{
  stc_withrottle_loco_listitem_t* pCurrent = pLocoList;
  if (pLocoList == NULL)
  {
    pLocoList = pLocoItem;
    pLocoList->pNextItem = NULL;
    return;
  }
  while(pCurrent->pNextItem != NULL)
  {
    pCurrent = pCurrent->pNextItem;
  }
  pCurrent->pNextItem = pLocoItem;
  pLocoItem->pNextItem = NULL;
}



/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
