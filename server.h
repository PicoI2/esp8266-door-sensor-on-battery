#pragma once

#include <ESP8266WebServer.h>

class CServer : public ESP8266WebServer {
  public:
    CServer ():ESP8266WebServer(80) {}
    void start();
    void loop();

  private:
    void handleRoot();
    void handleConfig();
    void handleReset();
    void handleState();
    void handleNotFound();

    bool mbStarted = false;
};

extern CServer Server;