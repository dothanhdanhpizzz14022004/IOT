#include <Arduino.h>
#include "sensor_data.h"
#include "led_temp_task.h"

#define LED_PIN GPIO_NUM_48

static TickType_t getHalfPeriod(float temperature) {
    if (temperature < 20.0f) {
        return pdMS_TO_TICKS(1000);   // 2000ms period -> 1000ms half
    } else if (temperature <= 30.0f) {
        return pdMS_TO_TICKS(500);    // 1000ms period -> 500ms half
    } else {
        return pdMS_TO_TICKS(125);    // 250ms period -> 125ms half
    }
}

void ledTempTask(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);

    bool ledState = false;
    TickType_t halfPeriod = pdMS_TO_TICKS(500);

    for (;;) {
        if (xSemaphoreTake(xNewDataSemaphore, 0) == pdTRUE) {
            SensorData_t data;
            if (xQueuePeek(xSensorQueue, &data, 0) == pdTRUE) {
                halfPeriod = getHalfPeriod(data.temperature);
                Serial.printf("[LED] Temp: %.1fC  Humidity: %.1f%%  Interval: %lums\n",
                              data.temperature, data.humidity, pdTICKS_TO_MS(halfPeriod) * 2);
            }
        }

        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        vTaskDelay(halfPeriod);
    }
}
