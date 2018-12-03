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
        Serial.printf("\n\n\nCLOSE WINDOW WITH SPEED %d\n\n\n",speed);
        Serial.printf("VALUES PIN1:%d,PIN2:%d\n\n",ledcRead(Pin1),ledcRead(Pin2));
        ledcWrite(1,0);
        ledcWrite(2,speed);
        Serial.printf("VALUES PIN1:%d,PIN2:%d",ledcRead(Pin1),ledcRead(Pin2));
        vTaskDelete(NULL);
    }

    static void open(void *){
        Serial.printf("\n\n\nOPEN WINDOW WITH SPEED %d\n\n\n",speed);
        Serial.printf("VALUES PIN1:%d,PIN2:%d\n\n",ledcRead(Pin1),ledcRead(Pin2));
        ledcWrite(1,speed);
        ledcWrite(2,0);
        Serial.printf("VALUES PIN1:%d,PIN2:%d",ledcRead(Pin1),ledcRead(Pin2));
        vTaskDelete(NULL);
    }

    static void stop(void* param) {
        Serial.printf("CREATED TASK STOP!!!!\n");
        ledcWrite(1,0);
        ledcWrite(2,0);
        vTaskDelete(NULL);
    }


    static void openStep(void*) {
        if(!openable||ledcRead(Pin1)||ledcRead(Pin2)){
            vTaskDelete(NULL);
            return;
        }
        openable = false;
        closeable = true;
        impulse = false;
        speedTemp=speed;
        do{
            ledcWrite(Pin2,0);
            ledcWrite(Pin1,(speedTemp+=1)%1023);
            Serial.printf("trying to open with speed of %d\n",speedTemp%1023);
            delay((1024-speedTemp)/20);
        }while(!impulse&&speedTemp<1000);
        speedTemp=260;
        while(impulse){
            ledcWrite(Pin2,speedTemp-=3);
            impulse = false;
            delay(400);
        }
        ledcWrite(Pin2,speedTemp+20);

        openable = speedTemp>980?false:true;

        ledcWrite(Pin1,openSilentSpeed);
        speedTemp = speed;
        int diff = abs(((int)(TempSensor::actualAndTargetTempDifference()*1000)));
        delay((5*diff));
        stop(NULL);
        vTaskDelete(NULL);
    }

    static void closeStep(void*) {
        if(!closeable||ledcRead(Pin1)||ledcRead(Pin2)){
            vTaskDelete(NULL);
            return;
        }
        closeable = false;
        openable = true;
        impulse = false;
        speedTemp = 230;
        do{
            ledcWrite(Pin1,0);
            ledcWrite(Pin2,(speedTemp+=1)%1023);
           Serial.printf("1trying to close with speed of %d\n",speedTemp%1023);
            delay((1024-speedTemp)/20);
        }while(!impulse&&speedTemp<1000);
        speedTemp=260;
        Serial.printf("2trying to close with speed of %d\n",speedTemp%1023);
        while(impulse){
            Serial.printf("3trying to close with speed of %d\n",speedTemp%1023);
            ledcWrite(Pin2,speedTemp-=3);
            impulse = false;
            delay(400);
        }
        Serial.printf("4trying to close with speed of %d\n",speedTemp%1023);
        ledcWrite(Pin2,speedTemp+20);



        closeable = speedTemp>980?false:true;
        if(speedTemp>300)speedTemp = 265;

        int diff = abs(((int)(TempSensor::actualAndTargetTempDifference()*1000)));
        delay(5*diff);
        stop(NULL);
        vTaskDelete(NULL);
    }


    static int getSpeed() {
        return speed;
    }
};


#endif //ESP12E_ACTUATOR_H
