#include "server.h"
#include "config.h"
#include "www/index.html.h"

CServer Server;

void CServer::start()
{
    on("/", [this]() { handleRoot(); });
    on("/config", [this]() { handleConfig(); });   // POST Request
    on("/reset", [this]() { handleReset(); });     // http://192.168.0.2/relay?state=on&time=2000
    onNotFound([this]() { handleNotFound(); });
    begin();
}

void CServer::handleRoot()
{
    send_P(200, "text/html", INDEX_HTML);
}

void CServer::handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += uri();
    message += "\nMethod: ";
    message += (method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += args();
    message += "\n";
    for (uint8_t i = 0; i < args(); i++) {
        message += " " + argName(i) + ": " + arg(i) + "\n";
    }
    send(404, "text/plain", message);
}

void CServer::handleConfig()
{
    for (uint8_t i = 0; i < args(); i++) {
        Config.setParamValue(argName(i).c_str(), arg(i));
        Config.save();
    }

    String message = "\
<!DOCTYPE HTML>\
<html lang='fr'>\
<head>\
  <title>Easy config</title>\
  <meta charset='utf-8'>\
</head>\
<body >\
  <h3>Configuration</h3>\
  <form method='post'>";
    message += Config.getParamsForm();
    message += "<input type='submit' value='Submit'>\
  </form><br>";

    message += "URI: ";
    message += uri();
    message += "\nMethod: ";
    message += (method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += args();
    message += "\n";
    message += "</body>\
</html>";

    send(200, "text/html", message);
}

void CServer::handleReset()
{
    Serial.println("reboot from http server");
    send(200, "text/html", "OK");
    ESP.reset();
}

void CServer::loop()
{   
    if (!mbStarted) {
        start();
        mbStarted = true;
    }
    
    // Handle HTTP client
    handleClient();
}