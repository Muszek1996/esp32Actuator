//
// Created by jakub on 24.10.18.
//

#ifndef ACTUATORV2_WIFI_H
#define ACTUATORV2_WIFI_H

#include <WiFi.h>

int a = 0;

namespace wifi{

    void printIP(){
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }





    void begin(){
    Serial.println();

    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin("401A","K0nr4f4l");
    }


    while (WiFi.status() != WL_CONNECTED) {
        if(a++==10)ESP.restart();
        delay(500);
        Serial.print(".");
    }

    printIP();
}

}


#endif //ACTUATORV2_WIFI_H
