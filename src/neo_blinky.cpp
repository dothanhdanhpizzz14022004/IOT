#include "neo_blinky.h"
#include "global.h"         
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

#define HUMI_LOW_THRESHOLD     40.0f
#define HUMI_NORMAL_THRESHOLD  70.0f

void updateNeoPixelColor(float humidity) {
    uint32_t color = strip.Color(0, 0, 0);  

    if (humidity < HUMI_LOW_THRESHOLD) {
        color = strip.Color(0, 0, 255);      // blue
    } 
    else if (humidity <= HUMI_NORMAL_THRESHOLD) {
        color = strip.Color(0, 255, 0);      // green 
    } 
    else { 
        color = strip.Color(255, 0, 0);     // red
    }

    strip.setPixelColor(0, color);
    strip.show();
}

void neo_blinky(void *pvParameters) {
    strip.begin();
    strip.setBrightness(200);
    strip.clear();
    strip.show();

    while (1) {
        if (xSemaphoreTake(xSemaphoreNeoPixel, pdMS_TO_TICKS(1000)) == pdTRUE) {
            if (glob_humidity > 0.0f) {
                updateNeoPixelColor(glob_humidity);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}