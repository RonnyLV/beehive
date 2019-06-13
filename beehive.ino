#include "beehive_main.h"

#include <Arduino.h>
#include <Ticker.h>
#include <HX711.h>
#include <DHTesp.h>

#include "integrations.h"

/* possible states */
typedef enum {
    SETUP = 1,
    RUNNING
} State;
State state = SETUP;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 13;
const int LOADCELL_SCK_PIN = 14;

const int LOADCELL2_DOUT_PIN = 2;
const int LOADCELL2_SCK_PIN = 4;

const unsigned int SLEEP_DURATION = (unsigned int) 20e6;

/** Initialize DHT sensor 1 */
DHTesp dhtSensor1;
/** Initialize DHT sensor 2 */
DHTesp dhtSensor2;

HX711 scale;
HX711 scale2;
/** Data from sensor 1 */
TempAndHumidity sensor1Data;
/** Data from sensor 2 */
TempAndHumidity sensor2Data;
/** Pin number for DHT11 1 data pin */
int dhtPin1 = 18;
/** Pin number for DHT11 2 data pin */
int dhtPin2 = 21;

void beginOperationBeeHive();

void setup() {
    Serial.begin(115200);
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);

    // Initialize temperature sensor 1
    dhtSensor1.setup(dhtPin1, DHTesp::DHT22);
    // Initialize temperature sensor 2
    dhtSensor2.setup(dhtPin2, DHTesp::DHT22);
    pinMode(5, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(35, OUTPUT);

    state = SETUP;
}

void loop() {
    switch (state) {
        case SETUP:
            state = RUNNING;
            break;
        case RUNNING:
            beginOperationBeeHive();
#if !DEBUG
            ESP.deepSleep(SLEEP_DURATION);
#endif
            break;
    }
}

void beginOperationBeeHive() {
    digitalWrite(5, HIGH);
    delay(100);
    digitalWrite(19, HIGH);
    delay(100);
    digitalWrite(35, HIGH);
    delay(2500);
    if (scale.is_ready() && scale2.is_ready()) {
        long reading = scale.read();
        long reading2 = scale2.read();
        debug(reading2);
        debug(" - ");
        debugln(reading);
    } else {
        debugln("HX711 not found.");
    }
    sensor1Data = dhtSensor1.getTempAndHumidity();  // Read values from sensor 1
    sensor2Data = dhtSensor2.getTempAndHumidity();  // Read values from sensor 1
    debugln("Sensor 1 data:");
    debugln(
            "Temp: " + String(sensor1Data.temperature, 2) + "'C Humidity: " + String(sensor1Data.humidity, 1) + "%");
    debugln("Sensor 2 data:");
    debugln(
            "Temp: " + String(sensor2Data.temperature, 2) + "'C Humidity: " + String(sensor2Data.humidity, 1) + "%");
    digitalWrite(5, LOW);
    digitalWrite(19, LOW);
    digitalWrite(35, LOW);
    delay(2000);

    sendFieldData(sensor1Data);
}
