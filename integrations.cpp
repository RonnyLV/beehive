#include <Arduino.h>
#include <WiFi.h>
#include <DHTesp.h>

#include "integrations.h"

#include "wifi.h"
#include "beehive_main.h"

const String THINGSPEAK_SERVER = "api.thingspeak.com"; // Server URL
const String THINGSPEAK_HTTP_HOST = "Host: api.thingspeak.com";
const String THINGSPEAK_API_KEY = "4PEIKQCLPMC3CGWM";

#if USE_TLS

#include <WiFiClientSecure.h>

/** api thingspeak root ca DigiCert */
const char *root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
"+OkuE6N36B9K\n" \
"-----END CERTIFICATE-----";
WiFiClientSecure thingspeakClient;
const int THINGSPEAK_PORT = 443;

#else

#include <WiFiClient.h>

WiFiClient thingspeakClient;
const int THINGSPEAK_PORT = 80;

#endif

void sendFieldData(TempAndHumidity sensor1Data) {
    setupNetwork();
#if USE_TLS
    thingspeakClient.setCACert(root_ca);
#endif

    debugln("\nStarting connection to server...");
    if (!thingspeakClient.connect(THINGSPEAK_SERVER.c_str(), THINGSPEAK_PORT)) {
        debugln("Connection failed!");
    } else {
        debugln("Connected to server!");
        // Make a HTTP request:
        thingspeakClient.println(
                "GET /update?api_key=" + THINGSPEAK_API_KEY +
                "&field1=" + String(sensor1Data.temperature, 2) +
                "&field2=" + String(sensor1Data.humidity, 1) +
                " HTTP/1.1"
        );
        thingspeakClient.println(THINGSPEAK_HTTP_HOST);
        thingspeakClient.println("Connection: close");
        thingspeakClient.println();

        while (thingspeakClient.connected()) {
            String line = thingspeakClient.readStringUntil('\n');
            if (line == "\r") {
                debugln("headers received");
                break;
            }
        }
        // if there are incoming bytes available
        // from the server, read them and print them:
        while (thingspeakClient.available()) {
            char c = thingspeakClient.read();
            debug(c);
        }

        thingspeakClient.stop();
    }

    WiFi.disconnect();
}
