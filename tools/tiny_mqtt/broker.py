"""Standalone hbmqtt broker on :1883, anonymous, topic-check enabled."""
import asyncio
import logging

from hbmqtt.broker import Broker

logging.basicConfig(level=logging.INFO, format="%(asctime)s %(name)s %(levelname)s %(message)s")

CONFIG = {
    "listeners": {
        "default": {"type": "tcp", "bind": "0.0.0.0:1883", "max_connections": 50},
    },
    "sys_interval": 10,
    "auth": {
        "allow-anonymous": True,
    },
    "topic-check": {
        "enabled": True,
        "plugins": ["topic_taboo"],
    },
}


async def _start():
    broker = Broker(CONFIG)
    await broker.start()


def run_forever():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    loop.run_until_complete(_start())
    loop.run_forever()


if __name__ == "__main__":
    run_forever()
