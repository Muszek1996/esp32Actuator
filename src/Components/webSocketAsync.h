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



struct tempSpeed{
    double temp;
    int speed;
};

namespace webSock {
    pthread_t pthread[2];
    static pthread_mutex_t mutexx;

    static void *reportTemperatures(void *param) {
        Serial.println("Reporting temps");
        if(!http::ws.count()){
            Serial.println("No clients not reporting shit to noone");
            return NULL;
        }
        Serial.println("Have clients reporting them shit");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & root = jsonBuffer.createObject();
        root["type"] = "TempSensorReadings";
        root["tempSlider"] = TempSensor::getTargetTemp();
        root["speedSlider"] = Actuator::getSpeed();
//        JsonArray &array = root.createNestedArray("data");
//        for (int j = 0; j < TempSensor::getICsQuantity(); j++) {
//            array.add((TempSensor::getLastTemps())[j]);
//        }
        size_t len = root.measureLength();
        AsyncWebSocketMessageBuffer * buffer = http::ws.makeBuffer(len);
        if (buffer){
            root.printTo((char *)buffer->get(), len + 1);
            http::ws.textAll(buffer);
        }
        Serial.println("reported temps to all msg sent sReturning");
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
            http::ws.printfAll("Setting temp to %f",tempReceived);
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
                        Serial.println("RECEIVED OPENsa MSG");
                        if(Actuator::openable){
                            Serial.println("Creating pthread");
                            pthread_create(&pthread[0],NULL,&Actuator::open,NULL);
                            Serial.println("Should be created already");
                        }

                        break;
                    }
                    case 0:{
                        Serial.println("RECEIVED CLOSExDD MSG");
                        if(Actuator::closeable)
                            pthread_create(&pthread[1],NULL,&Actuator::close,NULL);
                        break;
                    }
                    default:{
                        Serial.println("STOP");
                        Actuator::stop();
                        break;
                    }
                }
        }
        Serial.println("Reporting temps");
        reportTemperatures(NULL);
        return;
    }
    static void handleConnected(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
        Serial.printf("Connected");
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        client->printf("Hello Client %u :)", client->id());
        client->ping();
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

//        switch(type){
//            case WS_EVT_CONNECT:
//                handleConnected(server,client,type,arg,data,len);
//                break;
//            case  WS_EVT_DISCONNECT:
//                        handleDisconnected(server, client, type, arg, data, len);
//                break;
//            case WS_EVT_ERROR:
//                        handleError(server,client,type,arg,data,len);
//                break;
//            case WS_EVT_PONG:
//                        handlePong(server,client,type,arg,data,len);
//                break;
//            case WS_EVT_DATA:
//                        handleData(server,client,type,arg,data,len);
//                break;
//            default:
//
//                break;
//        }

            if(type == WS_EVT_CONNECT){

                handleConnected(server,client,type,arg,data,len);
            }else if(type == WS_EVT_DISCONNECT){
                handleDisconnected(server, client, type, arg, data, len);
            }else if(type == WS_EVT_ERROR){
                //Serial.printf("URL:%s\n",server->url());
                //Serial.printf("ID:%u\n",client->id());
                //Serial.printf("ARG:%u\n",*((uint16_t*)arg));
                //Serial.printf("DATA:%s\n",(char*)data);


                //Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
                //handleError(server,client,type,arg,data,len);
            }else if(type == WS_EVT_PONG){
                handlePong(server,client,type,arg,data,len);
            }else if(type == WS_EVT_DATA){
                    handleData(server,client,type,arg,data,len);
            }
        }



    static void begin(){
        http::ws.onEvent(onWsEvent);
        http::server.addHandler(&http::ws);
    }

    static void *checkTemperature(void *param) {
        double tempDifference = TempSensor::actualAndTargetTempDifference();
        if(tempDifference!=0){
            printf("Temp difference: %f\n", tempDifference);
            if(tempDifference>0){
                Serial.printf("Checked temps and need to close\n");
                xTaskCreate(&Actuator::closeStep,"tsk",100010,NULL,tskIDLE_PRIORITY+1,NULL);
               // pthread_create(&pthread[0],NULL,&Actuator::closeStep,NULL);
            }
            else{
               Serial.printf("Checked temps and need to open\n");
                pthread_create(&pthread[1],NULL,&Actuator::openStep,NULL);
            }
        }else
            Actuator::stop();
        return NULL;
    }
}


#endif //ESP32ACTUATOR_WEBSOCKETASYNC_H
