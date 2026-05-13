#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;

String WIFI_SSID = "abcde";
String WIFI_PASS = "12345678";
String CORE_IOT_TOKEN;
String CORE_IOT_SERVER;
String CORE_IOT_PORT;

bool led1State = false;
bool led2State = false;

boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();
SemaphoreHandle_t xSemaphoreNeoPixel = NULL; 

String glob_anomaly_status = "UNKNOWN";