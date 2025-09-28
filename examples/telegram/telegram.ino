#include <Arduino.h>
#include <time.h>
#include <ESP8266WiFi.h>
// #define TELEGRAM_SEND_DEBUG
// #define MAX_QUEUE_SIZE 20 //маск сообщений в очереди
#include <TelegramSend.h>
#include <Ticker.h>

const char *ssid = "You SSID";        // SSID WiFi network
const char *pass = "You wifi password"; // Password  WiFi network
const char *botToken = "You  telegram bot token";
int64_t userid = 123456789; //https://api.telegram.org/botYOUBOTTOKEN/getUpdates

TelegramSend Telegram;
Ticker ticker;

void newMsg()
{
  static u32 n = 0;
  if (Telegram.send(F("Привет №") + String(n), userid))
    n++;
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(500);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
  }
  Telegram.begin(botToken);
  ticker.attach(5, newMsg);
}

void loop(){Telegram.loop();}
