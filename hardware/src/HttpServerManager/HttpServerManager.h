#ifndef HTTPSERVERMANAGER_H
#define HTTPSERVERMANAGER_H

#include <ESPAsyncWebServer.h>

class HttpServerManager
{
public:
    HttpServerManager();
    void begin();
    bool hasChanges();
    String getSSID();
    String getPassword();

private:
    bool isRunning;
    bool changesRead;

    String ssid;
    String password;

    AsyncWebServer server;

    void serveHTML(AsyncWebServerRequest *request);
    void submitConfig(AsyncWebServerRequest *request);
    void deleteConfig(AsyncWebServerRequest *request);
};

#endif // HTTPSERVERMANAGER_H