#include "NetworkManager.h"

NetworkManager::NetworkManager(const char *apSSID)
{
    this->apSSID = apSSID;
}

void NetworkManager::begin()
{
    pref.begin("network", false);
    if (pref.isKey("ssid"))
    {
        Serial.print("Connecting to Wi-Fi: ");
        Serial.println(pref.getString("ssid", ""));
        WiFi.mode(WIFI_STA);
        String ssid = pref.getString("ssid", "");
        String password = pref.getString("password", "");
        this->ipAddress = pref.getString("ipAddress", "");
        this->isBroadcast = pref.getBool("isBroadcast", false);
        connectToWiFi(ssid, password);
    }
    else
    {
        Serial.println("Starting AP");
        WiFi.mode(WIFI_AP);
        Serial.println(apSSID);
        WiFi.softAP(apSSID);
    }
}

NetworkStatus NetworkManager::checkNetwork()
{
    NetworkStatus status = {
        .mode = WiFi.getMode(),
        .isConnected = WiFi.isConnected()};

    return status;
}

String NetworkManager::getIPAddress()
{
    if (WiFi.getMode() == WIFI_STA)
    {
        return WiFi.localIP().toString();
    }
    else if (WiFi.getMode() == WIFI_AP)
    {
        return WiFi.softAPIP().toString();
    }
    return String();
}

void NetworkManager::connectToWiFi(String ssid, String password)
{

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
}

void NetworkManager::deleteConfig()
{
    Serial.println("Deleting config");
    pref.clear();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID);
}

void NetworkManager::setConfig(String ssid, String password)

{
    Serial.println("Setting new config: ");
    Serial.print("SSID: ");
    Serial.println(ssid);

    if ((ssid == "" && pref.getString("ssid", "") != "") || (pref.getString("ssid", "") == ssid && pref.getString("password", "") == password))
    {
        Serial.println("Config is the same, not updating");
        return;
    }
    pref.putString("ssid", ssid);
    pref.putString("password", password);

    connectToWiFi(ssid, password);
}
