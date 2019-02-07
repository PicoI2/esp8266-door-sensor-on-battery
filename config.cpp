#include "config.h"
#include "param.h"
#include <EEPROM.h>
#include <ESP8266WiFi.h>

CConfig Config;

typedef void(commandFunction)(void);

struct SCommand {
    char* command;
    char* description;
    commandFunction* function;
};

SCommand CommandList[] = {
    {"help", "list commands", []() { Config.help(); }},
    {"param", "get/set parameter", []() { Config.param(); }},
    {"reset", "reset ESP8266", []() { Config.reset(); }},
    {"ip", "print ip address", []() { Config.ip(); }},
    {"mac", "print mac address", []() { Config.mac(); }},
};
#define COMMAND_NB (sizeof(CommandList) / sizeof(SCommand))

extern const char* version;
extern const char* wifi_ssid;
extern const char* wifi_pwd;
extern const char* url_low_state;
extern const char* url_high_state;
extern const char* sleep_duration;

const char* version = "Version";
const char* wifi_ssid = "WiFi SSID";
const char* wifi_pwd = "WiFi password";
const char* url_low_state = "Low state URL";
const char* url_high_state = "High state URL";
const char* sleep_duration = "Sleep duration in minutes, 0=infinite";

#define VERSION 1   // Increment me when modifying ParamList
CParam* ParamList[] = {
    new CParamByte(version),    // First param must be version
    new CParamString(wifi_ssid),
    new CParamString(wifi_pwd),
    new CParamString(url_low_state),
    new CParamString(url_high_state),
    new CParamByte(sleep_duration),
};
#define PARAM_NB (sizeof(ParamList) / sizeof(ParamList[0]))

// Constructor
CConfig::CConfig(void)
{
}

// Read config from eeprom
void CConfig::read(void)
{
    EEPROM.begin(4096); // Don't call it before setup()
    size_t Pos = 0;
    for (size_t i = 0; i < PARAM_NB; ++i) {
        ParamList[i]->read(Pos);
    }
    EEPROM.end();

    if (255 == getParamByteValue(version)) {
        Serial.println("First startup, writing default value");
        setParamValue(version, "1");
        setParamValue(wifi_ssid, "fill me");
        setParamValue(wifi_pwd, "fill me");
        setParamValue(url_low_state, "http://192.168.0.1/blablabla");
        setParamValue(url_high_state, "http://192.168.0.1/blablabla");
        setParamValue(sleep_duration, "60");
    }
}

// Save config to eeprom
void CConfig::save(void)
{
    EEPROM.begin(4096);
    size_t Pos = 0;
    for (size_t i = 0; i < PARAM_NB; ++i) {
        ParamList[i]->write(Pos);
    }
    EEPROM.end();
}

// Get a param value. Use only static const declaration of parameter name
String CConfig::getParamValue(const char* aName)
{
    String ret = "param not found";
    for (size_t i = 0; i < PARAM_NB; ++i) {
        if (aName == ParamList[i]->getName()) {
            ret = ParamList[i]->getValue();
            break;
        }
    }
    return ret;
}

// Get a param value. Use only static const declaration of parameter name
byte CConfig::getParamByteValue(const char* aName)
{
    byte ret = -1;
    for (size_t i = 0; i < PARAM_NB; ++i) {
        if (aName == ParamList[i]->getName()) {
            ret = ParamList[i]->getByteValue();
            break;
        }
    }
    return ret;
}

// Set a param value
void CConfig::setParamValue(const char* aName, String aValue)
{
    for (size_t i = 0; i < PARAM_NB; ++i) {
        if (0 == strcmp(aName, ParamList[i]->getName())) {
            ParamList[i]->setValue(aValue);
            break;
        }
    }
}

// Get form for all params
String CConfig::getParamsForm()
{
    String ret = "";
    for (size_t i = 1; i < PARAM_NB; ++i) {
        ret += ParamList[i]->toHtmlForm();
    }
    return ret;
}

// Function to call at each loop
void CConfig::loop(void)
{
    if (Serial.available()) {
        String readString = Serial.readString();
        mCommandLine += readString;

        // handle backspace
        if (0 < mCommandLine.indexOf('\b')) {
            mCommandLine.remove(mCommandLine.length() - 2, 2); // remove backspace from command line, and last character
        }

        // look for enter key
        if (0 <= mCommandLine.indexOf('\r') || 0 <= mCommandLine.indexOf('\n')) {
            Serial.println(readString);

            mCommandLine.remove(mCommandLine.length() - 1, 1); // remove last character (\r)

            size_t i;
            for (i = 0; i < COMMAND_NB; ++i) {
                if (0 == mCommandLine.indexOf(CommandList[i].command)) {
                    CommandList[i].function();
                    break;
                }
            }
            if (COMMAND_NB == i) {
                CommandList[0].function();
            }
            mCommandLine = "";
        }
        else {
            Serial.print(readString);
        }
    }
}

// List commands
void CConfig::help(void)
{
    Serial.println("Available commands:");
    for (size_t i = 0; i < COMMAND_NB; ++i) {
        Serial.print(CommandList[i].command);
        Serial.print('\t');
        Serial.println(CommandList[i].description);
    }
}

// Get/Set param
void CConfig::param(void)
{
    int index = mCommandLine.indexOf(' ');
    if (0 < index) {
        String args = mCommandLine.substring(index + 1);
        index = args.indexOf(' ');
        if (0 < index) {
            int paramNum = args.substring(0, index).toInt();
            String paramValue = args.substring(index + 1);
            if (paramNum < PARAM_NB) {
                ParamList[paramNum]->setValue(paramValue);
                save();
            }
        }
        else {
            Serial.println("Missing new value");
        }
    }
    Serial.println("Parameters:");
    for (size_t i = 1; i < PARAM_NB; ++i) {
        String line(i);
        line += String(" ") + ParamList[i]->getName() + "='"  + ParamList[i]->getValue() + "'";
        Serial.println(line);
    }
}

// Reset ESP
void CConfig::reset(void)
{
    Serial.println("Reset...");
    ESP.reset();
}

// Print IP address
void CConfig::ip(void)
{
    Serial.print("IP(sta): ");
    Serial.println(WiFi.localIP());
    Serial.print("IP(ap): ");
    Serial.println(WiFi.softAPIP());
}

// Print MAC address
void CConfig::mac(void)
{
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC: ");
    Serial.print(mac[5], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.println(mac[0], HEX);
}
