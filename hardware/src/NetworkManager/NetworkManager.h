#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <WiFi.h>
#include <Preferences.h>
#include "../HttpServerManager/HttpServerManager.h"

struct NetworkStatus
{
    WiFiMode_t mode;
    bool isConnected;
};

class NetworkManager
{
private:
    Preferences pref;
    const char *apSSID;
    bool isBroadcast;
    String ipAddress;

public:
    NetworkManager(const char *apSSID);
    void begin();
    NetworkStatus checkNetwork();
    String getIPAddress();
    void connectToWiFi(String ssid, String password);
    void setConfig(String ssid, String password);
    void deleteConfig();
};

#endif