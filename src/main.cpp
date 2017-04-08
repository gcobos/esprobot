#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "locomotion.h"
#include "credentials.h"

#define PIN_BUTTON  0

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

int button_status = 0;

void setup(void){
    WiFi.mode(WIFI_STA);

    Serial.begin(115200);

    IPAddress ip(192, 168, 8, 106);
    IPAddress gateway(192, 168, 8, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress dns(192, 168, 8, 1);
    WiFi.config(ip, dns, gateway, subnet);
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

    // Set GPIO0 s input (button)
    //pinMode(PIN_BUTTON, INPUT);
    //digitalWrite(12, 0);
    // digitalWrite(13, 1);
}

void loop(void)
{
    // Responds to web requests
    locomotion_handle_request();

    /*if (digitalRead(PIN_BUTTON)==0) {
        digitalWrite(12, ~button_status);
        digitalWrite(13, button_status);
        button_status = ~button_status;
        delay(500);
    }*/
}
