#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

typedef struct {
    float temperature;
    float humidity;
} SensorData_t;

typedef enum {
    STATE_NORMAL,
    STATE_WARNING,
    STATE_CRITICAL
} DisplayState_t;

extern QueueHandle_t xSensorQueue;
extern SemaphoreHandle_t xNormalSemaphore;
extern SemaphoreHandle_t xWarningSemaphore;
extern SemaphoreHandle_t xCriticalSemaphore;
extern SemaphoreHandle_t xI2CMutex;

#endif
