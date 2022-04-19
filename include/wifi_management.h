#include <WiFi.h>

IPAddress connectToWifi(const char*ssid, const char*password)
{
    Serial.println("Debut connexion au Wifi " + String(ssid));
    
    WiFi.begin(ssid, password);
    
    Serial.print("Connexion");
    // Tant que non connecté
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    // Connexion réussie
    Serial.println();
    Serial.println("Connexion OK");
    
    IPAddress adresseIP = WiFi.localIP();
    Serial.println("WiFi OK");
    Serial.print("Adresse IP : ");
    Serial.println(adresseIP);
    Serial.println("");
    return adresseIP;
}

void disconnectWiFi()
{
    WiFi.disconnect();
}

String macToStr(const uint8_t *mac)
{
    String result;
    for (int i = 0; i < 6; ++i)
    {
        result += String(mac[i], 16);
        if (i < 5)
            result += ':';
    }
    return result;
}