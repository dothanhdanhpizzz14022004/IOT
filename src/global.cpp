#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;

String WIFI_SSID = "abcde";
String WIFI_PASS = "12345678";
String CORE_IOT_TOKEN;
String CORE_IOT_SERVER;
String CORE_IOT_PORT;


boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();