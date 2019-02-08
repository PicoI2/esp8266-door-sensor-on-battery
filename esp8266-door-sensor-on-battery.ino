// Includes
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "config.h"
#include "server.h"

// Setup
void setup(void)
{
    Serial.begin(115200);
    Serial.setTimeout(10);
    Serial.println();
    Serial.println("Init");
    pinMode(0, INPUT);
    pinMode(14, INPUT);

    Config.read();

    // To enable WiFi
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);

    // If no DHCP
    if (0 == Config.getParamByteValue(use_dhcp)) {
        IPAddress ip, gateway, subnet;
        bool bRes = ip.fromString(Config.getParamValue(static_ip_address));
        bRes &= gateway.fromString(Config.getParamValue(static_ip_gateway));
        bRes &= subnet.fromString(Config.getParamValue(static_ip_subnet));
        if (bRes) {
            WiFi.config(ip, gateway, subnet);
        }
        else {
            Serial.println("Error while reading static IP parameters, using DHCP instead");
        }
    }

    WiFi.begin(Config.getParamValue(wifi_ssid).c_str(), Config.getParamValue(wifi_pwd).c_str());

    // Wait 5s max for WiFi
    for (int i=0; WL_CONNECTED != WiFi.status() && i<50; ++i) {
        Serial.println("Wait for WiFi");
        delay(100);
    }

    int Input0State = digitalRead(0);
    if (LOW == Input0State && WL_CONNECTED != WiFi.status()) {
        Serial.println("Unable to connect to WiFi, input 0 in low state, starting AccessPoint WiFI with SSID 'ESP8266-configure-me'");
        WiFi.mode(WIFI_OFF);
        WiFi.mode(WIFI_AP);
        WiFi.softAP("ESP8266-configure-me");
    }
}

// Loop
void loop(void)
{
    int Input0State = digitalRead(0);

    // Configuration mode
    if (LOW == Input0State) {
        Config.loop();
        Server.loop();
    }
    // Normal mode
    else if (WL_CONNECTED == WiFi.status()) {
        int DoorState = digitalRead(14);
        Serial.print("DoorState: ");
        Serial.println(DoorState);
            
        HTTPClient http;
        http.setTimeout(200);
        http.begin(Config.getParamValue(DoorState == HIGH ? url_high_state : url_low_state));
        int httpCode = http.GET();
        Serial.print("httpCode: ");
        Serial.println(httpCode);
        http.end();
    }
    else {
        Serial.println("Unable to connect to WiFi. Power me, wait for 2 seconds and connect input 0 to ground.");
        Serial.println("Then you will be able to configure me, by serial link or WiFi.");
    }

    Input0State = digitalRead(0);
    if (HIGH == Input0State) {
        uint64_t SleepDuration = (uint64_t)Config.getParamByteValue(sleep_duration) * 60 * 1000000;   // us
        if (0 == SleepDuration || SleepDuration >= ESP.deepSleepMax()) {
            SleepDuration = ESP.deepSleepMax();
        }
        Serial.print("I'm going to sleep for ");
        Serial.print((unsigned short)(SleepDuration / 60000000));
        Serial.println(" minutes");
        ESP.deepSleep(SleepDuration);
    }
}
