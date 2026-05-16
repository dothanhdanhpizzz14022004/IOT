# tools/

Python helpers for the local-MQTT / CoreIOT-gateway workflow (Videos 2–5).

## tiny_mqtt/
Local broker + listener, optional CoreIOT bridge. **Python 3.8** (3.9+ breaks hbmqtt 0.9.6 asyncio internals).

Setup with uv (recommended):
```
cd tools/tiny_mqtt
uv venv --python 3.8 .venv
uv pip install --python .venv/bin/python -r requirements.txt
```

Run:
```
./.venv/bin/python tiny_mqtt.py                 # broker + subscriber
./.venv/bin/python tiny_mqtt.py --with-gateway  # + forward to CoreIOT
./.venv/bin/python publisher.py                 # smoke test (sends "hello" every 5s)
```

Env for `--with-gateway`:
- `CORE_IOT_GW_TOKEN` — gateway access token from CoreIOT (Entities → Gateways → gear icon).

ESP firmware should publish to `gateway/telemetry` when `CORE_IOT_SERVER` points at the PC's LAN IP. Payload must include `deviceName`.

## coreiot_test/
Standalone scripts for Video 5 rule-chain testing. Bypass the gateway; talk to CoreIOT directly as two devices.

```
cd tools/coreiot_test
uv venv --python 3.8 .venv
uv pip install --python .venv/bin/python "paho-mqtt==1.6.1"
CORE_IOT_DEV1_TOKEN=... ./.venv/bin/python publisher.py   # ramps temperature
CORE_IOT_DEV2_TOKEN=... ./.venv/bin/python subscriber.py  # prints RPC calls
```
