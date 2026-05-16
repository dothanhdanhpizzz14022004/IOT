"""Dev publisher: sends `hello` every 5s to a test topic. Use to smoke-test broker+subscriber."""
import time
import paho.mqtt.client as mqtt

BROKER_HOST = "localhost"
BROKER_PORT = 1883
TOPIC = "test/hello"


def on_connect(client, userdata, flags, rc):
    print(f"[publisher] connected rc={rc}")


def on_publish(client, userdata, mid):
    print(f"[publisher] publish ack mid={mid}")


def run_forever():
    client = mqtt.Client(client_id="tiny-publisher")
    client.on_connect = on_connect
    client.on_publish = on_publish
    client.connect(BROKER_HOST, BROKER_PORT, 60)
    client.loop_start()
    try:
        while True:
            client.publish(TOPIC, "hello")
            print(f"[publisher] sent hello -> {TOPIC}")
            time.sleep(5)
    except KeyboardInterrupt:
        client.loop_stop()
        client.disconnect()


if __name__ == "__main__":
    run_forever()
