
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "htmlfs.h"

const char *ssid = "ssid";
const char *password = "password";

WebServer server(80);


typedef enum en_maerklin_ir_remote_address
{
  enMaerklinIrRemoteAddressC = 0x5,
  enMaerklinIrRemoteAddressD = 0xa,
} en_maerklin_ir_remote_address_t;


typedef enum en_maerklin_ir_remote_func
{
  enMaerklinIrRemoteFuncStop = 0,
  enMaerklinIrRemoteFuncBackward = 1,
  enMaerklinIrRemoteFuncForward = 2,
  enMaerklinIrRemoteFuncSound1 = 3,
  enMaerklinIrRemoteFuncSoundDoorClose = 3,
  enMaerklinIrRemoteFuncSoundCoupler = 3,
  enMaerklinIrRemoteFuncSound2 = 4,
  enMaerklinIrRemoteFuncSoundHorn = 4,
  enMaerklinIrRemoteFuncSound3 = 5,
  enMaerklinIrRemoteFuncSoundMotor = 5,
  enMaerklinIrRemoteFuncSoundAnnouncement = 5,
  enMaerklinIrRemoteFuncLight = 6,
} en_maerklin_ir_remote_func_t;

const uint16_t kIrLed = 12;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

uint16_t codeCache[17];

void setup() {
  // put your setup code here, to run once:

  //intiate serial port
  Serial.begin(115200);

  //initiate IR library
  irsend.begin();

  //initiate WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  HtmlFs_Init(&server);

  server.on("/cmd/{}/{}", []() {
    String cmd = server.pathArg(0);
    String cmdArg = server.pathArg(1);
    if (cmd == "sound")
    {
        if ((cmdArg == "motor") || (cmdArg == "3"))
        {
            sendMaerklin(enMaerklinIrRemoteAddressC,enMaerklinIrRemoteFuncSound1);
        } else if ((cmdArg == "horn") || (cmdArg == "2"))
        {
            sendMaerklin(enMaerklinIrRemoteAddressC,enMaerklinIrRemoteFuncSound2);
        } else if ((cmdArg == "coupler") || (cmdArg == "1"))
        {
            sendMaerklin(enMaerklinIrRemoteAddressC,enMaerklinIrRemoteFuncSound3);
        } 
    } else if (cmd == "speed")
    {
      int speed = cmdArg.toInt();
      sendMaerklin(enMaerklinIrRemoteAddressC,enMaerklinIrRemoteFuncStop);
      if (speed > 0)
      {
         while(speed != 0)
         {
            speed--;
            sendMaerklin(enMaerklinIrRemoteAddressC,enMaerklinIrRemoteFuncForward);
         }
      } else if (speed < 0)
      {
         while(speed != 0)
         {
            speed++;
            sendMaerklin(enMaerklinIrRemoteAddressC,enMaerklinIrRemoteFuncBackward);
         }
      } 
    }
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/cmd/{}", []() {
    String cmd = server.pathArg(0);
    if (cmd == "forward")
    {
        sendMaerklin(enMaerklinIrRemoteAddressC,enMaerklinIrRemoteFuncForward);
    } else if (cmd == "stop")
    {
        sendMaerklin(enMaerklinIrRemoteAddressC,enMaerklinIrRemoteFuncStop);
    }  else if (cmd == "backward")
    {
        sendMaerklin(enMaerklinIrRemoteAddressC,enMaerklinIrRemoteFuncBackward);
    } else if (cmd == "light")
    {
        sendMaerklin(enMaerklinIrRemoteAddressC,enMaerklinIrRemoteFuncLight);
    }
    server.send(200, "text/plain", "this works as well");
  });
  
  server.begin();
  Serial.println("HTTP server started");
  
}

void sendMaerklin(en_maerklin_ir_remote_address_t enAddress, en_maerklin_ir_remote_func_t enFunction)
{
  static bool bToggle = false;
  static uint8_t u8LastCommand = 0;
  uint8_t u8Command = (((uint8_t)enFunction & 0x7) << 4) | ((uint8_t)enAddress & 0xf);
  int i;
  if (u8Command != u8LastCommand)
  {
    bToggle = false;
  }
  u8LastCommand = u8Command;
  
  if (bToggle)
  {
    u8Command |= 0x80;
  }
  codeCache[0] = 5400;
  
  for(i = 0;i < 8;i++)
  {
    if ((u8Command & 0x80) != 0) 
    {
      codeCache[i*2 + 1] = 1700;
      codeCache[i*2 + 2] = 600;
    } else {
      codeCache[i*2 + 1] = 600;
      codeCache[i*2 + 2] = 1700;
    }
    u8Command = u8Command << 1;
  }
  irsend.sendRaw(codeCache,17,38);
  delay(10);
  bToggle = !bToggle;
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
