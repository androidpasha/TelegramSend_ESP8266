#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <queue>
#ifndef MAX_QUEUE_SIZE
#define MAX_QUEUE_SIZE 20
#endif

struct TelegramMsg
{
    u64 chatId;
    String text;
};

class TelegramSend
{
public:
    TelegramSend(){};
    TelegramSend(const char *botToken) { begin(botToken); }
    ~TelegramSend() { delete[] _botToken; }

    void begin(const char *botToken)
    {
        delete[] _botToken;
        _botToken = new char[strlen(botToken) + 1];
        strcpy(_botToken, botToken);
    }

    bool send(const String &message, u64 chatId)
    {
        if (message == "" || _queue.size() >= MAX_QUEUE_SIZE || chatId == 0)
        {
#ifdef TELEGRAM_SEND_DEBUG
            Serial.println("Overflov queue or message == \"\" or chatId == 0.");
#endif
            return false;
        }
        _queue.push({chatId, message});
        return true;
    }

    void loop()
    {
        if (!_queue.empty())
        {
            u16 httpCode = sendMessage(_queue.front().text, _queue.front().chatId);
#ifdef TELEGRAM_SEND_DEBUG
            Serial.printf("HTTP code = %d\n", httpCode);
#endif
            if (httpCode == 200)
                _queue.pop();
        }
    }

private:
    char *_botToken = nullptr;
    std::queue<TelegramMsg> _queue;

    u16 sendMessage(const String &message, u64 chatId)
    {
        if (message == "")
            return 200;
#ifdef TELEGRAM_SEND_DEBUG
        Serial.printf("Исходящее telegram сообщение: %s. В очереди: %d.", message.c_str(), _queue.size());
#endif

        BearSSL::WiFiClientSecure client;
        client.setInsecure();
        HTTPClient https;

        String url = String(F("https://api.telegram.org/bot")) + _botToken + F("/");
        if (https.begin(client, url))
        {
            https.addHeader(F("Content-Type"), F("application/json"));

            char *body = new char[52u + String(chatId).length() + message.length()];
            sprintf(body, (const char *)F(R"({"method":"sendMessage","chat_id":%llu,"text":"%s"})"),
                    chatId, message.c_str());

            s16 httpCode = https.POST(body);
            https.end();
            delete[] body;
            return httpCode;
        }
        else
            return 404;
    }
};
