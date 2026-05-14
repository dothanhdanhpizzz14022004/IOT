"""Tiny MQTT: run broker + subscriber (+ optional gateway bridge) in one process.

Usage:
    python tiny_mqtt.py                 # broker + subscriber only
    python tiny_mqtt.py --with-gateway  # + forward to CoreIOT (needs env CORE_IOT_GW_TOKEN)
"""
import argparse
import threading

import broker
import subscriber


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--with-gateway", action="store_true",
                        help="also run the CoreIOT gateway bridge thread")
    args = parser.parse_args()

    threads = [
        threading.Thread(target=broker.run_forever, name="broker", daemon=True),
        threading.Thread(target=subscriber.run_forever, name="subscriber", daemon=True),
    ]

    if args.with_gateway:
        import tiny_gateway
        threads.append(threading.Thread(target=tiny_gateway.run_forever,
                                        name="gateway", daemon=True))

    for t in threads:
        t.start()
        print(f"[tiny_mqtt] started thread: {t.name}")

    # Keep the main thread alive; daemon threads exit with process.
    for t in threads:
        t.join()


if __name__ == "__main__":
    main()
