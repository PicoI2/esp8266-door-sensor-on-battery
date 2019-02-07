#pragma once

#include <ESP8266WiFi.h>

class CConfig {
  public:
    CConfig();
    String getParamValue(const char* aName);
    byte getParamByteValue(const char* aName);
    void setParamValue(const char* aName, String aValue);
    String getParamsForm();
    void loop(void);
    void help(void);
    void param(void);
    void ip(void);
    void mac(void);
    void reset(void);
    void read(void);
    void save(void);

  private:
    String mCommandLine;
};

extern CConfig Config;

extern const char* version;
extern const char* wifi_ssid;
extern const char* wifi_pwd;
extern const char* url_low_state;
extern const char* url_high_state;
extern const char* sleep_duration;
