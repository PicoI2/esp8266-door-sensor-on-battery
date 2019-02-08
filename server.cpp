#include "server.h"
#include "config.h"
#include "www/index.html.h"

CServer Server;

void CServer::start()
{
    on("/", [this]() { handleRoot(); });
    on("/config", [this]() { handleConfig(); });
    on("/reset", [this]() { handleReset(); });
    on("/state", [this]() { handleState(); });
    onNotFound([this]() { handleNotFound(); });
    begin();
}

void CServer::handleRoot()
{
    send_P(200, "text/html", INDEX_HTML);
}

void CServer::handleNotFound()
{
    String response = "File Not Found\n\n";
    response += "URI: ";
    response += uri();
    response += "\nMethod: ";
    response += (method() == HTTP_GET) ? "GET" : "POST";
    response += "\nArguments: ";
    response += args();
    response += "\n";
    for (uint8_t i = 0; i < args(); i++) {
        response += " " + argName(i) + ": " + arg(i) + "\n";
    }
    send(404, "text/plain", response);
}

void CServer::handleConfig()
{
    for (uint8_t i = 0; i < args(); i++) {
        Config.setParamValue(argName(i).c_str(), arg(i));
        Config.save();
    }

    String response = "\
<!DOCTYPE HTML>\
<html lang='fr'>\
<head>\
    <title>ESP8266 Door sensor on battery</title>\
    <meta charset='utf-8'>\
</head>\
<body >\
    <h3>Configuration</h3>\
    <form method='post'>";
        response += Config.getParamsForm();
        response += "<input type='submit' value='Submit'>\
    </form>\
    </body>\
</html>";
    send(200, "text/html", response);
}

void CServer::handleState()
{
    int DoorState = digitalRead(14);

    String response = "\
<!DOCTYPE HTML>\
<html lang='fr'>\
<head>\
    <title>ESP8266 Door sensor on battery</title>\
    <meta charset='utf-8'>\
</head>\
<body >\
    <h3>State: ";
    response += DoorState;
    response += "</h3>\
</body>\
</html>";

    send(200, "text/html", response);
}

void CServer::handleReset()
{
    Serial.println("reboot from http server");
    send(200, "text/html", "Reset OK");
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