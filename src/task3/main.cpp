#include <Arduino.h>
#include <Wire.h>
#include "sensor_data.h"
#include "sensor_task.h"
#include "lcd_display_task.h"

QueueHandle_t xSensorQueue = NULL;
SemaphoreHandle_t xNormalSemaphore = NULL;
SemaphoreHandle_t xWarningSemaphore = NULL;
SemaphoreHandle_t xCriticalSemaphore = NULL;
SemaphoreHandle_t xI2CMutex = NULL;

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Task 3: LCD Temp/Humidity Monitor ===");

    Wire.begin(11, 12);

    xSensorQueue = xQueueCreate(1, sizeof(SensorData_t));
    xNormalSemaphore = xSemaphoreCreateBinary();
    xWarningSemaphore = xSemaphoreCreateBinary();
    xCriticalSemaphore = xSemaphoreCreateBinary();
    xI2CMutex = xSemaphoreCreateMutex();

    xTaskCreate(sensorTask, "SensorTask", 4096, NULL, 3, NULL);
    xTaskCreate(lcdDisplayTask, "LCDDisplayTask", 4096, NULL, 2, NULL);

    Serial.println("[Main] Tasks created. Scheduler running.");
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}
