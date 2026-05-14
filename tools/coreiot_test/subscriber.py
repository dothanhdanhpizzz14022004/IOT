"""Video 5 test subscriber: acts as device2, waits for RPC from rule chain.

Env:
    CORE_IOT_DEV2_TOKEN = access token of device2 on app.coreiot.io
"""
import json
import os

import paho.mqtt.client as mqtt

HOST = "app.coreiot.io"
PORT = 1883
RPC_REQUEST = "v1/devices/me/rpc/request/+"
RPC_RESPONSE_FMT = "v1/devices/me/rpc/response/{req_id}"

# Default device2 token (override via env CORE_IOT_DEV2_TOKEN)
DEFAULT_DEV2_TOKEN = "VdAvIBM8FD1VyhN8ErgH"


def run_forever():
    token = os.environ.get("CORE_IOT_DEV2_TOKEN", DEFAULT_DEV2_TOKEN).strip()
    if not token:
        raise RuntimeError("CORE_IOT_DEV2_TOKEN env var is required")

    def on_connect(client, userdata, flags, rc):
        print(f"[subscriber] connect rc={rc}")
        client.subscribe(RPC_REQUEST)

    def on_message(client, userdata, msg):
        req_id = msg.topic.rsplit("/", 1)[-1]
        try:
            body = json.loads(msg.payload.decode("utf-8"))
        except Exception:
            body = msg.payload
        print(f"[subscriber] RPC req_id={req_id} body={body}")
        # Ack so CoreIOT marks the call delivered
        client.publish(RPC_RESPONSE_FMT.format(req_id=req_id), json.dumps({"ack": True}))

    client = mqtt.Client(client_id="coreiot-sub-dev2")
    client.username_pw_set(token, "")
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(HOST, PORT, 60)
    client.loop_forever()


if __name__ == "__main__":
    run_forever()
