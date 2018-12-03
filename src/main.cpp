

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

void getTempsAndReport( void * parameter )
{
    /* loop forever */
    for(;;){
        TempSensor::getTemps(NULL);
        webSock::reportTemperatures(NULL);
        delay(4000);
    }
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

void checkImpulse(void *){
    for(;;){
        delay(1000);
        if(!Actuator::block)
        if(ledcRead(1)||ledcRead(2)){
            Actuator::impulse = false;
            delay(3000);
            if(!Actuator::impulse){
                Serial.print("\n\n\n\nNO IMPULSE SHUTTING DOWN AN ACTUATOR\n\n\n\n");
                ledcWrite(1,0);
                ledcWrite(2,0);
            }
        }
    }
}

void handleImpulse(){
    Serial.println("IMPULSE");
    Actuator::impulse = true;
}
static int tab[10];
unsigned long oneSec,tenSec;
void setup(void) {
    esp_log_level_set("*", ESP_LOG_ERROR);

    pthread_mutex_init(&webSock::mutexx,NULL);
    oneSec = 0;
    tenSec = 0;
    pinMode(36,INPUT);
    attachInterrupt(36, handleImpulse, CHANGE);
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

    xTaskCreatePinnedToCore(getTempsAndReport, "reportTemp", 10240, NULL, 1,NULL,1);
    xTaskCreatePinnedToCore(checkTemp, "checkTemp", 20480, NULL, 1,NULL,1);
    xTaskCreatePinnedToCore(checkImpulse, "checkImpulse", 10240, NULL, 1,NULL,1);
}


void loop(void) {
        ArduinoOTA.handle();
}

