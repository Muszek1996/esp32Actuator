//
// Created by jakub on 31.10.18.
//

#ifndef ESP32ACTUATOR_WEBSOCKETASYNC_H
#define ESP32ACTUATOR_WEBSOCKETASYNC_H
#include "http.h"
#include <LinkedList.h>
#include <AsyncJson.h>
#include <EEPROM.h>
#include <nvs.h>
#include "TempSensor/TempSensor.h"
#include "Actuator/Actuator.h"
#include "nvs.h"
#include <pthread.h>

AsyncWebSocket ws("/ws");

namespace webSock {


    static void *reportTemperatures(AsyncWebSocketClient * client) {
        TempSensor::getTemps(NULL);
        Serial.println("Reporting temps");
        Serial.println("Have clients reporting them shit");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & root = jsonBuffer.createObject();
        root["type"] = "TempSensorReadings";
        root["tempSlider"] = TempSensor::getTargetTemp();
        root["speedSlider"] = Actuator::getSpeed();
        JsonArray &array = root.createNestedArray("data");
           for (int j = 0; j < TempSensor::getICsQuantity(); j++) {
            array.add((TempSensor::getLastTemps())[j]);
        }
        size_t len = root.measureLength();
        AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len);
        if (buffer){
            root.printTo((char *)buffer->get(), len + 1);
            client->text(buffer);
        }
        Serial.println("reported temps to client");
        return NULL;
    }

    static void handleText(AsyncWebSocket *server, AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len){
        DynamicJsonBuffer jsonBuffer;
        JsonObject &root = jsonBuffer.parseObject(data);
        Serial.printf((char*)data);
        if(!root.success()){
            printf("Error while parsing JSON data");
            return;
        }
        Serial.println("SUCCES PARSING JSON\n");
        String type = root["type"].as<String>();
        Serial.printf("RECEIVED TYPE:%s\n",type.c_str());
        if(type=="tempOnSlider"){
            double tempReceived = root["data"].as<double>();
            printf("Setting temp to %f",tempReceived);
            TempSensor::setTargetTemp(tempReceived);
            Nvs::save(tempReceived);
        }else if(type == "speedOnSlider"){
            uint32_t speedReceived = root["data"].as<uint32_t>();
            Actuator::setSpeed(speedReceived);
            Nvs::save(speedReceived);
        }else if(type == "windowAction"){
            int dir = root["data"].as<int>();
            Serial.printf("CHAR: %d",dir);
                switch(dir){
                    case 1:{
                        Serial.printf("CREATING TASK OPEN!!!!\n");
                        Actuator::open();
                        break;
                    }
                    case 0:{
                        Serial.printf("CREATING TASK CLOSE!!!!\n");
                        Actuator::close();
                        break;
                    }
                    case 7:{
                        Serial.printf("CREATING TASK STOP!!!!\n");
                        Actuator::stop();
                        break;
                    }
                }
        }
        Serial.println("Reporting temps");
        reportTemperatures(client);
    }
    static void handleConnected(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
        Serial.printf("Connected");
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        client->printf("Hello Client %u :)", client->id());
        client->ping();
        reportTemperatures(client);
    }
    static void handleDisconnected(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
        Serial.printf("ws[%s] disconnect: %u\n", server->url(), client->id());
    }
    static void handleError(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    }
    static void handlePong(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *) data : "");
    }
    static void handleData(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
        AwsFrameInfo * info = (AwsFrameInfo*)arg;
        if(info->final && info->index == 0 && info->len == len) {
            Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(),
                          (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
            if (info->opcode == WS_TEXT) {
                handleText(server, client, arg, data, len);
            }
        }
    }

    static void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {

        switch(type){
            case WS_EVT_CONNECT:
                        handleConnected(server,client,type,arg,data,len);
                break;
            case WS_EVT_DISCONNECT:
                        handleDisconnected(server, client, type, arg, data, len);
                break;
            case WS_EVT_ERROR:
                        handleError(server,client,type,arg,data,len);
                break;
            case WS_EVT_PONG:
                        handlePong(server,client,type,arg,data,len);
                break;
            case WS_EVT_DATA:
                        handleData(server,client,type,arg,data,len);
                break;
            default:

                break;
        }


        }



    static void begin(){
       ws.onEvent(onWsEvent);
       http::addHandler(&ws);
    }

    static void checkTemperature(void *param) {

        double tempDifference = TempSensor::actualAndTargetTempDifference();
        if(tempDifference!=0){

           printf("Temp difference: %f\n", tempDifference);
            if(tempDifference>0){
                Serial.printf("Checked temps and need to close5Rev\n");

                if(digitalRead(39))
                Actuator::close5Rev();
                else{
                    Serial.println("Cant Close cuz closed");
                }
               // pthread_create(&pthread[0],NULL,&Actuator::closeStep,NULL);
            }
            else{
               Serial.printf("Checked temps and need to open5Rev\n");
               if(digitalRead(36))
                Actuator::open5Rev();
               else{
                   Serial.println("Cant Open cuz opened");
               }
               // xTaskCreate(Actuator::open5Rev,"openStep",4096,NULL,1,NULL);
            }
        }

    }
}


#endif //ESP32ACTUATOR_WEBSOCKETASYNC_H
