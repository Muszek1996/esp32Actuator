//
// Created by jakub on 31.10.18.
//

#ifndef ESP32ACTUATOR_NVS_H
#define ESP32ACTUATOR_NVS_H
#include <nvs_flash.h>
#include <nvs.h>
#include <esp_system.h>
#include "TempSensor/TempSensor.h"
#include "Actuator/Actuator.h"

namespace Nvs{
    static nvs_handle my_handle;
    static esp_err_t err;
    static int32_t speed;
    static int32_t temp;

static void begin() {
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_INVALID_VERSION) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

// Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");

// Read
        printf("Reading vals from NVS ... ");


        err = nvs_get_i32(my_handle, "temp", &temp);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("temp = %d\n", temp);
                TempSensor::targetTemp = ((double)temp)/100;
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        err = nvs_get_i32(my_handle, "speed", &speed);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("speed = %d\n", speed);
                Actuator::setSpeed(speed);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }


    }

}

    static void save(uint32_t a){
        printf("Updating speed in NVS ... ");
        err = nvs_set_i32(my_handle, "speed", a);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    }

    static void save(double a) {
        printf("Updating speed in NVS ... ");
        err = nvs_set_i32(my_handle, "temp", ((int32_t )(a*100)));
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    }


}



#endif //ESP32ACTUATOR_NVS_H
