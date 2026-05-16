#include <Arduino.h>
#include <Wire.h>
#include <DHT20.h>
#include "sensor_data.h"
#include "sensor_task.h"

void sensorTask(void *pvParameters) {
    Wire.begin(11, 12);
    DHT20 dht;
    dht.begin();

    vTaskDelay(pdMS_TO_TICKS(1000));

    for (;;) {
        dht.read();
        SensorData_t data;
        data.temperature = dht.getTemperature();
        data.humidity = dht.getHumidity();

        xQueueOverwrite(xSensorQueue, &data);
        xSemaphoreGive(xNewDataSemaphore);


        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
