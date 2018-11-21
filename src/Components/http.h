//
// Created by jakub on 24.10.18.
//

#ifndef ACTUATORV2_HTTP_H
#define ACTUATORV2_HTTP_H


//#include "../../../../../.platformio/packages/framework-arduinoespressif8266/libraries/ESP8266WebServer/src/ESP8266WebServer.h"
#include <WiFi.h>
#include <SPIFFS.h>   // Include the SPIFFS library
#include <ESPAsyncWebServer.h>

namespace http{

    static AsyncWebServer server(80);
    static AsyncWebSocket ws("/ws");
    static AsyncEventSource events("/events");


    static void begin() {
        // Initialize SPIFFS
        if(!SPIFFS.begin(true)){
            Serial.println("An Error has occurred while mounting SPIFFS");
            return;
        }                       // Start the SPI Flash Files System

        // Route for root / web page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/index.html", "text/html");
        });

        server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/favicon.ico", "image/ico");
        });

        // Route to load style.css file
        server.on("/WebSocket.js", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/WebSocket.js", "application/javascript");
        });

        server.begin();
    }

}

#endif //ACTUATORV2_HTTP_H
