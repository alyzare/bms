#include <Arduino.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include "DisplayManager/DisplayManager.h"
#include "NetworkManager/NetworkManager.h"
#include "HttpServerManager/HttpServerManager.h"
#include "WebSocketManager/WebSocketManager.h"
#include "SensorUtils/SensorUtils.h"
#include "LightsManager/LightsManager.h"
#include <Preferences.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define INTERVAL 200
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ2PIN 34

DisplayManager display;
NetworkManager network("ESP32-AP");
HttpServerManager httpServer;
WebSocketManager *webSocketManager;
LightsManager lightsManager;
WiFiUDP udp;
DHT dht(DHTPIN, DHTTYPE);

uint32_t prevMillis = 0;
uint16_t wifiFailCounter = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting ESP32");
  display.begin();
  delay(2000);
  network.begin();
  httpServer.begin();
  dht.begin();
  Serial.println("ESP32 started");
}

void loop()
{
  unsigned long currentMillis = millis();

  if (prevMillis > currentMillis)
  {
    prevMillis = currentMillis;
  }

  if (currentMillis - prevMillis >= INTERVAL)
  {
    prevMillis = currentMillis;

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    float gas = calculateGasPercentage(analogRead(MQ2PIN));

    if (httpServer.hasChanges())
    {
      Serial.println("Changes detected");

      httpServer.getSSID();
      if (httpServer.getSSID() != "")
      {
        network.setConfig(httpServer.getSSID(), httpServer.getPassword());
      }
      else
      {
        Serial.println("Client deleted config!");
        network.deleteConfig();
      }
    }

    NetworkStatus status = network.checkNetwork();
    if (status.mode == WIFI_MODE_AP)
    {
      display.updateNetwork(WIFI_MODE_AP, false);
      display.showIP(network.getIPAddress());
    }
    else
    {
      display.updateNetwork(WIFI_MODE_STA, status.isConnected);
      display.showIP(network.getIPAddress());

      if (status.isConnected)
      {
        String payload = "BMS_HARDWARE";
        udp.beginPacket("255.255.255.255", 1380);
        udp.write((uint8_t *)payload.c_str(), strlen(payload.c_str()));
        udp.endPacket();
        if (webSocketManager == nullptr)
        {
          Serial.println("Creating broadcast WebSocketManager");
          webSocketManager = new WebSocketManager(81);
        }
      }
      else
      {
        if (webSocketManager != nullptr)
        {
          Serial.println("Deleting WebSocketManager");
          delete webSocketManager;
          webSocketManager = nullptr;
          Serial.println("WebSocketManager deleted and set to nullptr");
        }
        wifiFailCounter++;
        if (wifiFailCounter >= 50)
        {
          Serial.println("Wi-Fi not found!");
          network.deleteConfig();
        }
      }

      if (webSocketManager != nullptr)
      {
        JsonDocument doc;
        doc["temperature"] = temp;
        doc["humidity"] = hum;
        doc["gas"] = gas;
        doc["lightStatus"] = lightsManager.getState();
        String jsonData;
        serializeJson(doc, jsonData);
        webSocketManager->sendData(jsonData.c_str());

        if (webSocketManager->hasOrder())
        {
          lightsManager.changeState(webSocketManager->getOrder());
        }
      }
    }
    display.drawLights(lightsManager.getState());
  }
}
