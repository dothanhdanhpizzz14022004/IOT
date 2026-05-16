#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "sensor_data.h"
#include "lcd_display_task.h"

#define LCD_ADDR 0x21
#define LCD_COLS 16
#define LCD_ROWS 2

void lcdDisplayTask(void *pvParameters) {
    LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

    if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {
        lcd.begin();
        lcd.backlight();
        lcd.setCursor(0, 0);
        lcd.print("Initializing...");
        xSemaphoreGive(xI2CMutex);
    }

    for (;;) {
        const char *stateStr;

        if (xSemaphoreTake(xCriticalSemaphore, 0) == pdTRUE) {
            stateStr = "!! CRITICAL !!";
        } else if (xSemaphoreTake(xWarningSemaphore, 0) == pdTRUE) {
            stateStr = "* WARNING *";
        } else if (xSemaphoreTake(xNormalSemaphore, pdMS_TO_TICKS(500)) == pdTRUE) {
            stateStr = "  NORMAL";
        } else {
            continue;
        }

        SensorData_t data;
        if (xQueuePeek(xSensorQueue, &data, 0) != pdTRUE) {
            continue;
        }

        if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            lcd.clear();
            char line1[17];
            snprintf(line1, sizeof(line1), "T:%.1fC H:%.1f%%", data.temperature, data.humidity);
            lcd.setCursor(0, 0);
            lcd.print(line1);
            lcd.setCursor(0, 1);
            lcd.print(stateStr);
            xSemaphoreGive(xI2CMutex);
        }

        Serial.printf("[LCD] %s | T:%.1fC H:%.1f%%\n", stateStr, data.temperature, data.humidity);
    }
}
