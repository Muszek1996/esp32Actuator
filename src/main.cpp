

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
bool val = true;

void getTempsAndReport( void * parameter )
{
    /* loop forever */
    for(;;){
        TempSensor::getTemps(NULL);
        webSock::reportTemperatures(NULL);
        delay(4000);
    }
    delay(50);
    vTaskDelete( NULL );
}
void checkTemp( void * parameter )
{
    /* loop forever */
    for(;;){
        delay(8000);
        webSock::checkTemperature(NULL);
    }
    /* delete a task when finish,
    this will never happen because this is infinity loop */
    vTaskDelete( NULL );
}
void blink(void *){
    for(;;){
        val=!val;
        if(val){
            digitalWrite(2,HIGH);
        }else{
            digitalWrite(2,LOW);
        }
        delay(1000);
    }
    vTaskDelete(NULL);
}



void checkImpulse(void *){
    for(;;){
        delay(1000);
        if(!Actuator::block){
            Actuator::impulse = false;
            delay(5000);
            if(!Actuator::impulse){
                Serial.print("\n\n\n\nNO IMPULSE SHUTTING DOWN AN ACTUATOR\n\n\n\n");
                ledcWrite(1,0);
                ledcWrite(2,0);
            }
        }
    }
}

void handleImpulse(){
    if(!Actuator::impulse){
        Actuator::impulse = true;
        Serial.println("IMPULSE");
    }

}
static int tab[10];
unsigned long oneSec,tenSec;
void setup(void) {
    pinMode(2,OUTPUT);
    pinMode(34,OUTPUT);
    digitalWrite(34,LOW);
    esp_log_level_set("*", ESP_LOG_ERROR);

    pthread_mutex_init(&webSock::mutexx,NULL);
    oneSec = 0;
    tenSec = 0;
    pinMode(36,INPUT);
    analogSetWidth(12);
    attachInterrupt(digitalPinToInterrupt(36), handleImpulse, FALLING);
    ledcAttachPin(25,1);
    ledcAttachPin(26,2);
    ledcSetup(1, 20000, 10);
    ledcSetup(2, 20000, 10);
    Serial.begin(115200);

    Nvs::begin();
    TempSensor::begin();

    wifi::begin();
    webSock::begin();
    http::begin();
    ota::begin();
    //TODO pinned to core if err;

    xTaskCreate(getTempsAndReport, "reportTemp", 10240, NULL, 1,NULL);
    xTaskCreate(blink, "blink", 2048, NULL, 1,NULL);
    xTaskCreate(checkTemp, "checkTemp", 20480, NULL, 1,NULL);
    xTaskCreate(checkImpulse, "checkImpulse", 10240, NULL, 1,NULL);
}


void loop(void) {
    //Serial.printf("IMPULSE:%d\n",analogRead(36));
        ArduinoOTA.handle();
}

