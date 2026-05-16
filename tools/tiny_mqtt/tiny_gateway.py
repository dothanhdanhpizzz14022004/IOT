"""Tiny Gateway: subscribes to local broker, forwards to CoreIOT as Gateway telemetry.

Requires gateway access token in env var CORE_IOT_GW_TOKEN.
Create the gateway on CoreIOT: Entities -> Gateways -> Add Gateway (profile=default),
then open it -> gear icon -> copy access token.

Message shape accepted from local devices:
    topic:   gateway/telemetry   (or any topic; payload is what matters)
    payload: {"deviceName": "yolo-xxxx", "temperature": 25.0, "humidity": 60.0}

Forwarded to CoreIOT:
    topic:   v1/gateway/telemetry
    payload: {"<deviceName>": [{"ts": <epoch_ms>, "values": {"temperature": 25.0, "humidity": 60.0}}]}
CoreIOT auto-creates devices on first message.
"""
import json
import os
import time

import paho.mqtt.client as mqtt

LOCAL_HOST = "localhost"
LOCAL_PORT = 1883
LOCAL_TOPIC = "gateway/#"

COREIOT_HOST = "app.coreiot.io"
COREIOT_PORT = 1883
COREIOT_TOPIC = "v1/gateway/telemetry"

# Default gateway token (override via env CORE_IOT_GW_TOKEN)
DEFAULT_GW_TOKEN = "wFXGmj1nSdj41bZLUrDX"


def _build_cloud_client():
    token = os.environ.get("CORE_IOT_GW_TOKEN", DEFAULT_GW_TOKEN).strip()
    if not token:
        raise RuntimeError("CORE_IOT_GW_TOKEN env var is required")
    client = mqtt.Client(client_id="tiny-gateway-cloud")
    client.username_pw_set(token, "")
    client.connect(COREIOT_HOST, COREIOT_PORT, 60)
    client.loop_start()
    return client


def run_forever():
    cloud = _build_cloud_client()

    def on_connect(client, userdata, flags, rc):
        print(f"[gateway] local connect rc={rc}")
        client.subscribe(LOCAL_TOPIC)

    def on_message(client, userdata, msg):
        try:
            body = json.loads(msg.payload.decode("utf-8"))
        except Exception as e:
            print(f"[gateway] skip non-JSON on {msg.topic}: {e}")
            return
        device = body.pop("deviceName", None)
        if not device:
            print(f"[gateway] skip payload without deviceName: {body}")
            return
        envelope = {
            device: [{
                "ts": int(time.time() * 1000),
                "values": body,
            }]
        }
        cloud.publish(COREIOT_TOPIC, json.dumps(envelope))
        print(f"[gateway] -> {device}: {body}")

    local = mqtt.Client(client_id="tiny-gateway-local")
    local.on_connect = on_connect
    local.on_message = on_message
    local.connect(LOCAL_HOST, LOCAL_PORT, 60)
    local.loop_forever()


if __name__ == "__main__":
    run_forever()
