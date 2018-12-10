//
// Created by Jakub on 06.10.2018.
//

#ifndef ESP12E_ACTUATOR_H
#define ESP12E_ACTUATOR_H


#include <Arduino.h>
#include "../webSocketAsync.h"
#include <pthread.h>


namespace Actuator {
    static uint32_t speed = 250;
    static const uint8_t Pin1 = 1;
    static const uint8_t Pin2 = 2;
    static unsigned int openSilentSpeed = 300;
    static unsigned int closeSilentSpeed = 300;
    static bool openable = true;
    static bool closeable = true;
    static volatile bool impulse;
    static void stop();
    static bool block = false;
    static uint32_t speedTemp = speed;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    static bool isImpulse(){
        bool impTmp = impulse;
        impulse = false;
        return impTmp;
    }

    static void setSpeed(int speed) {
        Actuator::speed = speed;
        if(ledcRead(1)){
            ledcWrite(1,speed);
        }else if(ledcRead(2)){
            ledcWrite(2,speed);
        }
    }

    static void close(void *){
        ledcWrite(1,0);
        ledcWrite(2,speed);
        vTaskDelete(NULL);
    }

    static void open(void *){
        ledcWrite(1,speed);
        ledcWrite(2,0);
        vTaskDelete(NULL);
    }

    static void stop(void* param) {
        ledcWrite(1,0);
        ledcWrite(2,0);
        vTaskDelete(NULL);
    }


    static void openStep(void*) {
        if((!ledcRead(1))&&(!ledcRead(2)))block = false;
        Serial.printf("Openable:%d LedcRead(1):%dLedcRead(2):%d block:%d\n",openable,ledcRead(1),ledcRead(2),block);
        if((!openable)||ledcRead(Pin1)||ledcRead(Pin2)||block){
            Serial.println("CANNOT open openable FALSE");
            delay(50);
            vTaskDelete(NULL);
            return;
        }
        block = true;
        openable = false;
        closeable = true;
        impulse = false;
        speedTemp=speed;
        do{
            ledcWrite(Pin2,0);
            ledcWrite(Pin1,(speedTemp+=1)%1023);
            Serial.printf("trajing to open with speed of %d\n",speedTemp);
            delay((1425-speedTemp)/20);
        }while(!impulse&&speedTemp<1224);

        Serial.printf("SpeedTemp:%d, < 1204 : %d\n",speedTemp,speedTemp<=1204);
        openable = speedTemp<=1204;
        Serial.printf("openable : %d\n",openable);
        speedTemp = 280;
        while(impulse){
            Serial.printf("3trying to open with speed of %d\n",speedTemp%1023);
            ledcWrite(Pin1,speedTemp-=5);
            impulse = false;
            delay(2000);
        }
        Serial.printf("4trying to open with speed of %d\n",(speedTemp%1023)+15);
        ledcWrite(Pin1,speedTemp+15);
        speedTemp = speed;
        block = false;
        int diff = abs(((int)(TempSensor::actualAndTargetTempDifference()*1000)));
        delay((5*diff+1000));
        Serial.println("BLOCK = FALSE");
        stop(NULL);
        delay(50);
        vTaskDelete(NULL);
    }

    static void closeStep(void*) {
        if((!ledcRead(1))&&(!ledcRead(2)))block = false;
        Serial.printf("CLOSEABLE:%d\n",closeable);
        if((!closeable)||ledcRead(Pin1)||ledcRead(Pin2)||block){
            Serial.println("CANNOT CLOSE CLOEABLE FALSE");
            delay(50);
            vTaskDelete(NULL);
            return;
        }
        block = true;
        Serial.println("CLOSING WINDOW");
        closeable = false;
        openable = true;
        impulse = false;
        speedTemp = 230;
        do{
            ledcWrite(Pin1,0);
            ledcWrite(Pin2,(speedTemp+=1)%1023);
           Serial.printf("1trying to close with speed of %d\n",speedTemp%1023);
            delay((1425-speedTemp)/20);
        }while(!impulse&&speedTemp<1224);
        closeable = speedTemp<=1204;
        speedTemp = 280;
        while(impulse){
            Serial.printf("3trying to close with speed of %d\n",speedTemp%1023);
            ledcWrite(Pin2,speedTemp-=5);
            impulse = false;
            delay(2000);
        }
        Serial.printf("4trying to close with speed of %d\n",(speedTemp%1023)+35);
        ledcWrite(Pin2,speedTemp+35);
        speedTemp = speed;
        block = false;
        int diff = abs(((int)(TempSensor::actualAndTargetTempDifference()*1000)));
        delay(5*diff+1000);
        stop(NULL);
        delay(50);
        vTaskDelete(NULL);
    }


    static int getSpeed() {
        return speed;
    }
};


#endif //ESP12E_ACTUATOR_H
