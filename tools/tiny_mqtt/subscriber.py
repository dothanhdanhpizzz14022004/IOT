"""Paho subscriber: listens on `#` on local broker, prints topic + payload."""
import time
import paho.mqtt.client as mqtt

BROKER_HOST = "localhost"
BROKER_PORT = 1883
TOPIC = "#"


def on_connect(client, userdata, flags, rc):
    print(f"[subscriber] connected rc={rc}")
    client.subscribe(TOPIC)


def on_subscribe(client, userdata, mid, granted_qos):
    print(f"[subscriber] subscribed mid={mid} qos={granted_qos}")


def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode("utf-8", errors="replace")
    except Exception:
        payload = repr(msg.payload)
    print(f"[subscriber] topic={msg.topic} payload={payload}")


def run_forever():
    client = mqtt.Client(client_id="tiny-subscriber")
    client.on_connect = on_connect
    client.on_subscribe = on_subscribe
    client.on_message = on_message
    for i in range(10):
        try:
            client.connect(BROKER_HOST, BROKER_PORT, 60)
            break
        except ConnectionRefusedError:
            print(f"[subscriber] broker not ready, retry {i+1}/10")
            time.sleep(1)
    client.loop_forever()


if __name__ == "__main__":
    run_forever()
