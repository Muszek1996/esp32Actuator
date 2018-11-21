//
// Created by Jakub on 06.10.2018.
//

#ifndef ESP12E_TEMPSENSOR_H
#define ESP12E_TEMPSENSOR_H

#include <cstdint>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include "../http.h"


namespace TempSensor {
    static const int tempBias = -2;
    static const uint8_t iCsQuantity = 2;
    static double *lastTemps = new double[2];
    static double getTargetTemp();
    static void setTargetTemp(double targetTemp);
    static double targetTemp = 22;
    static const double tolerance = 0.05;
    static OneWire oneWire(15);
    static DallasTemperature sensors(&oneWire);
    static const uint8_t getICsQuantity();
    static double getAverageTemp();
    static pthread_mutex_t mutex;

    static double *getLastTemps();

    static double actualAndTargetTempDifference(){   // zwraca nadwyżkę temperatury+/niedobór-;
        double averageTemp = getAverageTemp();
        if(averageTemp>(targetTemp+tolerance)||averageTemp<(targetTemp-tolerance))return ((targetTemp+tolerance)-averageTemp);
        else return 0;
    }

    static void begin(){
        sensors.begin();
        pthread_mutex_init(&mutex,NULL);
    }





    static void *getTemps(void *param){
        sensors.requestTemperatures();
        for(uint8_t i=0;i<iCsQuantity;++i){
            double temp = (sensors.getTempCByIndex(iCsQuantity-1-i)+tempBias);
            if(temp>0&&temp<40){
                if(i==0){
                    lastTemps[i]=(temp-1);
                }else{
                    lastTemps[i]=temp;
                }
            }
            else{
                return NULL;
            }
        }
        return NULL;
    }

    static double getAverageTemp(){
        getTemps(NULL);
        double temp=0;
        for(int i=0;i<iCsQuantity;i++){
            temp+=lastTemps[i];
        }
        temp/=iCsQuantity;
        return temp;
    }

    static double getMyTempCByIndex(uint8_t i){
        if(i<0||i>iCsQuantity) return -126.6969;
        sensors.requestTemperatures();
        lastTemps[i]=(sensors.getTempCByIndex(iCsQuantity-1-i)+tempBias);
        return lastTemps[i];
    }



    double getTargetTemp() {
        return targetTemp;
    }

    void setTargetTemp(double tTemp) {
        pthread_mutex_lock(&mutex);
        http::ws.printfAll("Setter temp to %f",tTemp);
        targetTemp = tTemp;
        pthread_mutex_unlock(&mutex);
    }

    double *getLastTemps() {
        return lastTemps;
    }

    const uint8_t getICsQuantity() {
        return iCsQuantity;
    }



};


#endif //ESP12E_TEMPSENSOR_H
