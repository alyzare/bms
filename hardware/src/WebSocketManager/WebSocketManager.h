#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>

class WebSocketManager
{
public:
    WebSocketManager(uint16_t port);

    ~WebSocketManager();

    void sendData(const char *data);

    bool hasOrder();

    uint8_t getOrder();

private:
    bool hasOrderFlag = false;
    uint8_t order = 0;
    
    AsyncWebSocket *serverSocket;
    AsyncWebServer *server;
};

#endif