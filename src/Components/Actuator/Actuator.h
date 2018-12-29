//
// Created by Jakub on 06.10.2018.
//

#ifndef ESP12E_ACTUATOR_H
#define ESP12E_ACTUATOR_H


#include <Arduino.h>
#include "../webSocketAsync.h"
#include <pthread.h>
#include <Stepper.h>

static TaskHandle_t xHandle = NULL;
static TaskHandle_t closeHandle = NULL;
static TaskHandle_t openHandle = NULL;


static Stepper stepper(200, 13, 12, 14, 27);

static void closeTillClosed(void *);
static void closeV(void *);
static void openTillOpened(void *);
static void openV(void *);
static void stopV(void* );

namespace Actuator {


    static void stop(){
        xTaskCreate(stopV,"stop",4096,NULL,1,NULL);
    }
    static void setSpeed(int speed) {
        stepper.setSpeed(speed);
    }

    static int getSpeed() {
        return stepper.getSpeed();
    }

    static void close(){
        stop();
        xTaskCreate(closeTillClosed,"close",4096,NULL,1,&xHandle);
    }

    static void open(){
        stop();
        xTaskCreate(openTillOpened,"open",4096,NULL,1,&xHandle);
    }


    static void close5Rev(){
        Serial.printf("xhandle : %d",xHandle);
        if(xHandle == NULL)
        xTaskCreate(closeV,"close5Rev",4096,NULL,1,&xHandle);
        else Serial.printf("HANDLE BUSY");
    }

    static void open5Rev(){
        Serial.printf("xhandle : %d",xHandle);
        if(xHandle == NULL)
        xTaskCreate(openV,"open5Rev",4096,NULL,1,&xHandle);
        else Serial.printf("HANDLE BUSY");
    }



};


static void closeTillClosed(void *){
    stepper.setSpeed(Actuator::getSpeed());

    while(digitalRead(39)&&xHandle)
        stepper.step(1000);
    closeHandle = NULL;
    vTaskDelete(NULL);
}

static void closeV(void *){
    stepper.setSpeed(Actuator::getSpeed());
    if(digitalRead(39))
        stepper.step(1000);
    xHandle = NULL;
    vTaskDelete(NULL);
}

static void openTillOpened(void *){
    stepper.setSpeed(Actuator::getSpeed());
    while(digitalRead(36)&&xHandle)
        stepper.step(-1000);
    openHandle = NULL;
    vTaskDelete(NULL);
}
static void openV(void *){
    stepper.setSpeed(Actuator::getSpeed());
    if(digitalRead(36))
        stepper.step(-1000);
    xHandle = NULL;
    vTaskDelete(NULL);
}

static void stopV(void* ) {
    if(xHandle){
        vTaskDelete(xHandle);
        xHandle = NULL;
    }
    stepper.stop();
    vTaskDelete(NULL);
}



#endif //ESP12E_ACTUATOR_H

