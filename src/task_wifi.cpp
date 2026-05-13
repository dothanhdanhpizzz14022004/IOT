#include "task_wifi.h"
#include "global.h"

void startAP()
{
    Serial.println("Dang khoi tao AP...");
    WiFi.mode(WIFI_AP);
    if (WiFi.softAP("YOLOUNO_10", "12345678")){
    // if (WiFi.softAP(WIFI_SSID.c_str(), WIFI_PASS.c_str())) {
        Serial.println("AP Mode successful!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("AP Mode failed!");
    }
}

void startSTA()
{
    if (WIFI_SSID.isEmpty())
    {
        vTaskDelete(NULL);
    }

    WiFi.mode(WIFI_STA);

    if (WIFI_PASS.isEmpty())
    {
        WiFi.begin(WIFI_SSID.c_str());
    }
    else
    {
        WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    //Give a semaphore here
    xSemaphoreGive(xBinarySemaphoreInternet);
}

bool Wifi_reconnect()
{
    const wl_status_t status = WiFi.status();
    if (status == WL_CONNECTED)
    {
        return true;
    }
    startSTA();
    return false;
}
