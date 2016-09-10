#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "locomotion.h"

const char* ssid = "AndroidAP";
const char* password = "htbd5992";

void setup(void){
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Prepare scheduler and web service for the locomotion
    locomotion_init(WiFi.localIP());
}

void loop(void)
{
    // Responds to web requests
    locomotion_handle_request();
}
