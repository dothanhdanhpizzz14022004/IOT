#include <Arduino.h>
#include "sensor_data.h"
#include "sensor_task.h"
#include "led_temp_task.h"

QueueHandle_t xSensorQueue = NULL;
SemaphoreHandle_t xNewDataSemaphore = NULL;

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Task 1: LED Blink with Temperature ===");

    xSensorQueue = xQueueCreate(1, sizeof(SensorData_t));
    xNewDataSemaphore = xSemaphoreCreateBinary();

    xTaskCreate(sensorTask, "SensorTask", 4096, NULL, 3, NULL);
    xTaskCreate(ledTempTask, "LEDTempTask", 4096, NULL, 2, NULL);

    Serial.println("[Main] Tasks created. Scheduler running.");
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}
