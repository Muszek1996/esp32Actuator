

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
#include <Stepper.h>
bool val = true;


void checkTemp( void * parameter )
{
    /* loop forever */
    for(;;){
        delay(1000);
        double diff = abs(TempSensor::actualAndTargetTempDifference());
        int waitTime = static_cast<int>(999.0/diff);
        if(diff){
            Serial.printf("CHECKING TEMP DIFF IS (%f) DELAY IS(%d)\n",diff,waitTime);
            delay(waitTime);
            webSock::checkTemperature(NULL);
        }
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




void setup(void) {
    Serial.begin(115200);

    pinMode(2,OUTPUT);  // 1 s blinking diode
    pinMode(36,INPUT);  //end close window
    pinMode(39,INPUT);  //endsensor open window

    wifi::begin();
    Nvs::begin();
    TempSensor::begin();
    http::begin();
    webSock::begin();

    xTaskCreate(blink, "blink", 4096, NULL, tskIDLE_PRIORITY+1,NULL);
    xTaskCreate(checkTemp, "checkTemp", 20480, NULL, tskIDLE_PRIORITY+1,NULL);
}


void loop(void) {

}
