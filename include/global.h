#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern float glob_temperature;
extern float glob_humidity;

#define HUMI_LOW_THRESHOLD      40.0f
#define HUMI_NORMAL_THRESHOLD   70.0f

//TASK NEO PIXEL COLOR DEFINITIONS
#define COLOR_DRY       255, 0, 0      // BLUE - DRY
#define COLOR_NORMAL    0, 255, 0      // GREEN - NORMAL
#define COLOR_WET       0, 0, 255      // RED - WET

extern String WIFI_SSID;
extern String WIFI_PASS;
extern String CORE_IOT_TOKEN;
extern String CORE_IOT_SERVER;
extern String CORE_IOT_PORT;

extern boolean isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet;
extern SemaphoreHandle_t xSemaphoreNeoPixel;
  
extern bool led1State;
extern bool led2State;

extern String glob_anomaly_status;

#endif