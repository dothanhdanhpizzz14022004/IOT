#include <Arduino.h>
#include <Wire.h>
#include <DHT20.h>
#include "sensor_data.h"
#include "sensor_task.h"

static DisplayState_t classifyState(float temp, float hum) {
    if (temp < 15.0f || temp > 35.0f || hum < 30.0f || hum > 80.0f) {
        return STATE_CRITICAL;
    }
    if (temp < 20.0f || temp > 30.0f || hum < 40.0f || hum > 70.0f) {
        return STATE_WARNING;
    }
    return STATE_NORMAL;
}

void sensorTask(void *pvParameters) {
    DHT20 dht;

    if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {
        dht.begin();
        xSemaphoreGive(xI2CMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    for (;;) {
        SensorData_t data;

        if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            dht.read();
            data.temperature = dht.getTemperature();
            data.humidity = dht.getHumidity();
            xSemaphoreGive(xI2CMutex);
        } else {
            Serial.println("[Sensor] Failed to acquire I2C mutex");
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        xQueueOverwrite(xSensorQueue, &data);

        DisplayState_t state = classifyState(data.temperature, data.humidity);
        const char *stateStr;
        switch (state) {
            case STATE_CRITICAL:
                xSemaphoreGive(xCriticalSemaphore);
                stateStr = "CRITICAL";
                break;
            case STATE_WARNING:
                xSemaphoreGive(xWarningSemaphore);
                stateStr = "WARNING";
                break;
            default:
                xSemaphoreGive(xNormalSemaphore);
                stateStr = "NORMAL";
                break;
        }

        Serial.printf("[Sensor] Temp: %.1fC  Humidity: %.1f%%  State: %s\n",
                      data.temperature, data.humidity, stateStr);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
