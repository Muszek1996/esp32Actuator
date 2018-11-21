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
    static bool openable = true;
    static bool closeable = true;
    static volatile bool impulse;
    static void stop();
    static uint32_t speedTemp = speed;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


    static void setSpeed(int speed) {
        Actuator::speed = speed;
    }

    static void *close(void *){
        Serial.println("CLOSING!!!");
        if(!closeable)return NULL;
        closeable = false;
        speedTemp = speed;
        impulse = false;
        ledcWrite(Pin1,0);
        do{
            ledcWrite(Pin2,(speedTemp+=1)%1023);
            delay(100);
        }while(!impulse&&speedTemp<1000);

        while(impulse){
            impulse = false;
            ledcWrite(Pin2,speedTemp-30);
            http::ws.printfAll("CLOSING WHILE IMPULSE %d",speedTemp-30);
            delay(1000);
        }
        closeable = false;
        speedTemp = speed;

        return NULL;
    }

    static void *open(void *){
        Serial.println("OPENING!!!");
        if(!openable){
            Serial.println("NON OPENABLE RETURN NULL");
            return NULL;
        }
        openable = false;
        speedTemp = speed;
        impulse = false;
        ledcWrite(Pin2,0);
        Serial.println("EXECUTING DO WHILE");
        do{
            Serial.println("IN DO WHILE ALREADY");
            delay(0);
            ledcWrite(Pin1,(speedTemp+=1)%1023);
            delay(100);
        }while(!impulse&&speedTemp<1000);
        Serial.println("EXITED DO WHILE");

        while(impulse){
            impulse = false;
            ledcWrite(Pin1,speedTemp-30);
            http::ws.printfAll("OPENING WHILE IMPULSE %d",speedTemp-30);
            delay(1000);
        }
        openable = false;
        speedTemp = speed;

        return NULL;
    }




    static void open() {
        if(!openable)return;
        openable = false;
        uint32_t speedTemp = speed-10;
        impulse = false;
        do{
            ledcWrite(Pin2,0);
            ledcWrite(Pin1,(speedTemp+=10)%1023);
            Serial.printf("trying to open with speed of %d\n",speedTemp%1023);
            delay(400);
        }while(!impulse&&speedTemp<1000);

        if(speedTemp>980){
            openable = false;
        }
        else{
            openable = true;
        }
    }
    static void* openStep(void*) {
        if(!openable)return NULL;
        openable = false;

        impulse = false;
        do{
            delay(0);
            ledcWrite(Pin2,0);
            ledcWrite(Pin1,(speedTemp+=1)%1023);
           Serial.printf("trying to open with speed of %d\n",speedTemp%1023);
            delay(100);
        }while(!impulse&&speedTemp<1000);
        ledcWrite(Pin1,265);
        speedTemp = speed-25;
        if(speedTemp>980){
            openable = false;
        }
        else{
            openable = true;
        }
        delay(1000);
        stop();
        return NULL;
    }


    static void close() {
        if(!closeable)return;
        closeable = false;
        uint32_t speedTemp = speed-10;
        impulse = false;
        do{
            ledcWrite(Pin1,0);
            ledcWrite(Pin2,(speedTemp+=10)%1023);
            Serial.printf("trying to close with speed of %d\n",speedTemp%1023);
            delay(400);
        }while(!impulse&&speedTemp<1000);
        if(speedTemp>980){
            closeable = false;
        }
        else{
            closeable = true;
        }
    }
    static void closeStep(void*) {
        if(!closeable) vTaskDelete(NULL);
        closeable = false;
        impulse = false;
        do{
            delay(0);
            ledcWrite(Pin1,0);
            ledcWrite(Pin2,(speedTemp+=1)%1023);
           Serial.printf("trying to close with speed of %d\n",speedTemp%1023);
            delay(100);
        }while(!impulse&&speedTemp<1000);
        ledcWrite(Pin2,265);
        speedTemp = speed-25;
        if(speedTemp>980){
            closeable = false;
        }
        else{
            closeable = true;
        }
        delay(1000);
        stop();
        vTaskDelete(NULL);
    }

    static void stop() {
        openable = true;
        closeable = true;
        ledcWrite(Pin1,0);
        ledcWrite(Pin2,0);
    }

    static int getSpeed() {
        return speed;
    }
};


#endif //ESP12E_ACTUATOR_H
