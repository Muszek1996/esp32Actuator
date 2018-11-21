

#include "Components/wifi.h"
#include "Components/http.h"
#include "Components/ws.h"
#include "Components/ota.h"
#include "Components/webSocketAsync.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <esp_system.h>



void print(){
    Actuator::impulse = true;
    http::ws.printfAll("Impulse");
}

unsigned long oneSec,tenSec;
void setup(void) {



    pthread_mutex_init(&webSock::mutexx,NULL);
    oneSec = 0;
    tenSec = 0;
    pinMode(36,INPUT);
    attachInterrupt(36,print,CHANGE);
    ledcAttachPin(2,1);
    ledcAttachPin(4,2);
    ledcSetup(1, 20000, 10);
    ledcSetup(2, 20000, 10);
    Serial.begin(115200);

    Nvs::begin();
    TempSensor::begin();



    wifi::begin();
    webSock::begin();
    http::begin();
    ota::begin();

}


void loop(void) {
        ArduinoOTA.handle();

    if(millis()> oneSec + 1000){
        Serial.printf("main millis:%ld\n",millis());
        ota::handle();
        TempSensor::getTemps(NULL);
        //pthread_create(&pthread[0],NULL,TempSensor::getTemps,NULL);
        //xTaskCreate(TempSensor::getTemps,"getTemp",100000,NULL,tskIDLE_PRIORITY+1,NULL);
        //xTaskCreate(webSock::reportTemperatures,"rpTemp",100000,NULL,tskIDLE_PRIORITY+1,NULL);
        //pthread_create(&pthread[1],NULL,webSock::reportTemperatures,NULL);
        webSock::reportTemperatures(NULL);
        oneSec = millis();
    }

    if(millis()> tenSec + 10000){
        webSock::checkTemperature(NULL);
        Serial.printf("Temp");
        //pthread_create(&pthread[2],NULL,webSock::checkTemperature,NULL);
        //xTaskCreate(webSock::checkTemperature,"ckTemp",100000,NULL,tskIDLE_PRIORITY+1,NULL);
        tenSec = millis();
    }


}

