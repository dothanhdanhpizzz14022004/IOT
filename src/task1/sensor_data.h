#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

typedef struct {
    float temperature;
    float humidity;
} SensorData_t;

extern QueueHandle_t xSensorQueue;
extern SemaphoreHandle_t xNewDataSemaphore;

#endif
