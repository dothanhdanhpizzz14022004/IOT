"""Video 5 test publisher: ramps temperature to trigger rule-chain RPC.

Connects directly to CoreIOT as device1 (the sensor). Starts at 26C, +2C every 2s.
Rule chain sends RPC to device2 when temp >= 50 (ON) or temp < 30 (OFF).

Env:
    CORE_IOT_DEV1_TOKEN = access token of device1 on app.coreiot.io
"""
import json
import os
import time

import paho.mqtt.client as mqtt

HOST = "app.coreiot.io"
PORT = 1883
TOPIC = "v1/devices/me/telemetry"

# Default device1 token (override via env CORE_IOT_DEV1_TOKEN)
DEFAULT_DEV1_TOKEN = "mPBEzAIldBLOsSwWtp2I"


def run_forever():
    token = os.environ.get("CORE_IOT_DEV1_TOKEN", DEFAULT_DEV1_TOKEN).strip()
    if not token:
        raise RuntimeError("CORE_IOT_DEV1_TOKEN env var is required")

    client = mqtt.Client(client_id="coreiot-pub-dev1")
    client.username_pw_set(token, "")
    client.connect(HOST, PORT, 60)
    client.loop_start()

    temp = 26.0
    try:
        while True:
            payload = json.dumps({"temperature": temp, "humidity": 55.0})
            client.publish(TOPIC, payload)
            print(f"[publisher] temp={temp} payload={payload}")
            temp += 2.0
            if temp > 60:
                temp = 26.0
            time.sleep(2)
    except KeyboardInterrupt:
        client.loop_stop()
        client.disconnect()


if __name__ == "__main__":
    run_forever()
