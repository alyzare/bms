#include "WebSocketManager.h"
#include <HTTPClient.h>

WebSocketManager::WebSocketManager(uint16_t port)
{
    serverSocket = new AsyncWebSocket("/");
    serverSocket->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
                          {
        if (type == WS_EVT_CONNECT)
        {
            Serial.printf("Client connected: %u\n", client->id());

            client->text("hello");
        }
        else if (type == WS_EVT_DISCONNECT)
        {
            Serial.printf("Client disconnected: %u\n", client->id());
        }
        else if (type == WS_EVT_DATA)
        {
            if (len >= 6 && data[0] == '\r' && data[1] == '\n' && data[2] == '\r' && data[3] == '\n')
            {
                Serial.printf("Received data from client: %u\n", client->id());
                bool valid = true;
                for (size_t i = 4; i < 8; ++i)
                {
                    if (data[i] != '0' && data[i] != '1')
                    {
                        valid = false;
                        break;
                    }
                }
                if (valid)
                {
                  this-> order = (data[4] - '0') << 3 | (data[5] - '0') << 2 | (data[6] - '0') << 1 | (data[7] - '0');
                  this-> hasOrderFlag = true;
                  Serial.printf("Received order: %u\n", order);
                }
                else
                {
                    Serial.printf("Received invalid data from client: %u\n", client->id());
                }
            }
        } });

    server = new AsyncWebServer(port);
    server->addHandler(serverSocket);
    server->begin();
}
WebSocketManager::~WebSocketManager()
{
    delete server;
}

void WebSocketManager::sendData(const char *data)
{
    for (auto &client : serverSocket->getClients())
    {
        client.text(data);
    }
}

bool WebSocketManager::hasOrder()
{
    return hasOrderFlag;
}

uint8_t WebSocketManager::getOrder()
{
    return order;
}
