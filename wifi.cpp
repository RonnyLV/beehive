#include <WiFi.h>

#if DEBUG

#include "wifi_debug.h"

#endif

#include "wifi.h"

const char *WIFI_SSID = "afk-brb";
const char *WIFI_PASSWORD = "skrubisskrubis";

void setupNetwork() {
#if DEBUG
    scanWirelessNetworks();
#endif

    Serial.print(F("Setup_Start"));
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFiClass::status() != WL_CONNECTED) {
        Serial.print("retrying\n" + String(WiFiClass::status()));
        delay(500);
    }

    Serial.print(F("Connected. My IP address is: "));
    Serial.println(WiFi.localIP());
}

