#include "task_wifi.h"
#include "global.h"
#include "mainserver.h"     // ← Thêm dòng này
#include <WiFi.h>

const char* AP_SSID = "YOLOUNO_10";
const char* AP_PASS = "12345678";

void wifi_task(void *pvParameters)
{
    Serial.println(" WiFi Task Started...");

    // Khởi tạo Access Point
    startAP();

    // Khởi tạo Web Server từ mainserver.cpp
    setupServer();        // Hàm này đã có trong mainserver.cpp

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));   // Kiểm tra định kỳ
    }
}

// ====================== ACCESS POINT ======================
void startAP()
{
    Serial.println("Access Point...");
    WiFi.mode(WIFI_AP);
    
    if (WiFi.softAP(AP_SSID, AP_PASS)) {
        Serial.println(" AP Mode Started");
        Serial.print("SSID: ");
        Serial.println(AP_SSID);
        Serial.print("Password: ");
        Serial.println(AP_PASS);
        Serial.print(" IP Address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println(" AP Mode Failed");
    }
}


void startSTA()
{
    if (WIFI_SSID.isEmpty())
    {
        Serial.println("No SSID configured for STA mode");
        vTaskDelete(NULL);
    }

    WiFi.mode(WIFI_STA);
    if (WIFI_PASS.isEmpty())
        WiFi.begin(WIFI_SSID.c_str());
    else
        WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());

    Serial.println("Connecting to WiFi STA...");

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        Serial.print(".");
    }

    Serial.println("\n Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    xSemaphoreGive(xBinarySemaphoreInternet);
}

bool Wifi_reconnect()
{
    if (WiFi.status() == WL_CONNECTED) return true;
    
    Serial.println("Reconnecting to WiFi...");
    startSTA();
    return false;
}