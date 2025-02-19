#include "HttpServerManager.h"
#include "Constants.h"
#include <iostream>

HttpServerManager::HttpServerManager() : server(80) // Initialize server with port 80
{
}

void HttpServerManager::begin()
{
    Serial.println("Starting HTTP server");
    if (!isRunning)
    {
        server.on("/config", HTTP_GET, [this](AsyncWebServerRequest *request)
                  { serveHTML(request); });
        server.on("/config", HTTP_POST, [this](AsyncWebServerRequest *request)
                  { submitConfig(request); });
        server.on("/config", HTTP_DELETE, [this](AsyncWebServerRequest *request)
                  { deleteConfig(request); });

        server.begin();
        Serial.println("HTTP server started");
        isRunning = true;
        changesRead = true;
    }
}

bool HttpServerManager::hasChanges()
{
    if (changesRead)
        return false;
    changesRead = true;
    return true;
}

String HttpServerManager::getSSID()
{
    Serial.println(ssid);
    return ssid;
}

String HttpServerManager::getPassword()
{
    Serial.println(password);
    return password;
}

void HttpServerManager::serveHTML(AsyncWebServerRequest *request)
{
    request->send(200, "text/html", htmlText);
    Serial.println("Served HTML to: ");
    Serial.println(request->client()->remoteIP().toString());
}

void HttpServerManager::submitConfig(AsyncWebServerRequest *request)
{
    Serial.println("Received configuration from: ");
    Serial.println(request->client()->remoteIP().toString());
    for (int i = 0; i < request->params(); i++)
    {
        const AsyncWebParameter *p = request->getParam(i);
        Serial.print(p->name());
        Serial.print(": ");
        Serial.println(p->value());
    }

    if (request->hasParam("ssid", true))
    {
        ssid = request->getParam("ssid", true)->value();
        password = request->getParam("password", true)->value();

        Serial.println("Configuration submitted successfully");
        Serial.print("SSID: ");
        Serial.println(ssid);

        changesRead = false;

        request->send(200, "text/plain", "Configuration submitted successfully");
    }
    else
    {
        Serial.println("Invalid request");
        request->send(400, "text/plain", "Invalid request");
    }
}

void HttpServerManager::deleteConfig(AsyncWebServerRequest *request)
{
    ssid = "";
    password = "";

    changesRead = false;

    Serial.println("Configuration deleted");
    request->send(200, "text/plain", "Configuration deleted successfully");
}
