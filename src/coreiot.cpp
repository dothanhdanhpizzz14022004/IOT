#include "coreiot.h"

WiFiClient espClient;
PubSubClient client(espClient);

// Decide local broker vs CoreIOT cloud by the server address
static bool is_cloud_server() {
  return CORE_IOT_SERVER.indexOf("coreiot.io") >= 0;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print(CORE_IOT_SERVER);
    Serial.print(":");
    Serial.println(CORE_IOT_PORT);

    String clientId = "ESP32Client-";
    clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

    bool ok;
    if (is_cloud_server() && CORE_IOT_TOKEN.length() > 0) {
      // CoreIOT: MQTT username = access token, empty password
      ok = client.connect(clientId.c_str(), CORE_IOT_TOKEN.c_str(), "");
    } else {
      // Local broker: anonymous (matches Tiny MQTT config)
      ok = client.connect(clientId.c_str());
    }

    if (ok) {
      Serial.println("connected to MQTT broker!");
      client.subscribe("v1/devices/me/rpc/request/+");
      Serial.println("Subscribed to v1/devices/me/rpc/request/+");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  // Allocate a temporary buffer for the message
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  Serial.print("Payload: ");
  Serial.println(message);

  // Parse JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* method = doc["method"];
  if (strcmp(method, "setStateLED") == 0) {
    // Check params type (could be boolean, int, or string according to your RPC)
    // Example: {"method": "setValueLED", "params": "ON"}
    const char* params = doc["params"];

    if (strcmp(params, "ON") == 0) {
      Serial.println("Device turned ON.");
      //TODO

    } else {   
      Serial.println("Device turned OFF.");
      //TODO

    }
  } else {
    Serial.print("Unknown method: ");
    Serial.println(method);
  }
}


void setup_coreiot(){

  //Serial.print("Connecting to WiFi...");
  //WiFi.begin(wifi_ssid, wifi_password);
  //while (WiFi.status() != WL_CONNECTED) {
  
  // while (isWifiConnected == false) {
  //   delay(500);
  //   Serial.print(".");
  // }

  while(1){
    if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY)) {
      break;
    }
    delay(500);
    Serial.print(".");
  }


  Serial.println(" Connected!");

  client.setServer(CORE_IOT_SERVER.c_str(), CORE_IOT_PORT.toInt());
  client.setCallback(callback);

}

void coreiot_task(void *pvParameters){

    setup_coreiot();

    while(1){

        if (!client.connected()) {
            reconnect();
        }
        client.loop();

        // Telemetry payload. Include deviceName so Tiny Gateway can demux local-broker traffic.
        String deviceName = "yolo-" + String((uint32_t)ESP.getEfuseMac(), HEX);
        String payload = "{\"deviceName\":\"" + deviceName +
                         "\",\"temperature\":" + String(glob_temperature) +
                         ",\"humidity\":" + String(glob_humidity) + "}";

        // Cloud CoreIOT expects device-scoped topic; local gateway listens on a custom topic.
        const char* topic = is_cloud_server()
            ? "v1/devices/me/telemetry"
            : "gateway/telemetry";
        client.publish(topic, payload.c_str());


        
        Serial.println("Published payload: " + payload);
        vTaskDelay(10000);  // Publish every 10 seconds
    }
}