# IoT Course — Final Submission (YoloUNO ESP32-S3)

All six course tasks plus Lab 4 in a single PlatformIO project. Tasks 1, 3, and 6 are isolated build environments selected by `make taskN`. Tasks 2, 4, 5 live inside the Task 6 / Lab 4 unified build.

## Hardware

- **Board**: YoloUNO ESP32-S3 (native USB)
- **DHT20** (temp+humi) on I2C: SDA=GPIO11, SCL=GPIO12, addr 0x38
- **LCD 1602 I2C** addr 0x21 (Task 3)
- **Onboard LED** GPIO48 (Task 1)
- **NeoPixel** onboard (Task 2 / Task 6)
- **I2S mic** (Task 5 TinyML)

## Quick start

Install [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html), then:

```bash
make help          # list all targets
make task1         # build + flash Task 1
make task3         # build + flash Task 3
make task6         # build + flash Task 6 / Lab 4 (also uploads LittleFS)
make monitor       # serial monitor @ 115200
```

Build only (no flash): `make build1`, `make build3`, `make build6`.

## Tasks

| # | What it does | RTOS primitives |
|---|--------------|-----------------|
| 1 | Temperature-driven LED blink — three speed bands (cold / normal / hot) | `xQueueOverwrite` + binary semaphore signaling new sensor data |
| 2 | NeoPixel color by humidity (dry/normal/wet) — runs inside Task 6 build | shared global, polled |
| 3 | LCD displays temp/humi + classification (NORMAL / WARNING / CRITICAL) | I2C mutex shared between sensor and LCD task; per-state semaphores |
| 4 | AP-mode web server with WebSocket LED/NeoPixel control — runs inside Task 6 build | command queues |
| 5 | TinyML keyword spotting (TFLite Micro + I2S) — runs inside Task 6 build | result queue + audio-ready semaphore |
| 6 | **Lab 4**: full IoT gateway — local MQTT broker → CoreIOT cloud bridge | binary semaphore for WiFi state; MQTT client task |

### Source layout

```
src/
├── (root .cpp files)         # Task 6 / Lab 4 build (default)
├── task1/                    # Task 1 isolated build
└── task3/                    # Task 3 isolated build
include/                      # headers for Task 6 build
data/                         # web UI assets (LittleFS, Task 6)
lib/                          # vendored libs (DHT20, LCD, PubSubClient, ThingsBoard, ElegantOTA, etc.)
tools/                        # Lab 4 Python tooling
```

`platformio.ini` uses `build_src_filter` per env so each task only compiles its own `.cpp` files.

---

## Lab 4 (Task 6) — IoT Gateway Demo

Goal: demonstrate the **gateway pattern** — N sensors → 1 local hub → cloud, plus server-side logic via CoreIOT rule chain.

### Architecture

```
[YoloUNO S3 #1] ─┐
[YoloUNO S3 #2] ─┼─ WiFi ─▶ [PC: Tiny MQTT broker] ──▶ [PC: Tiny Gateway] ──MQTT──▶ [CoreIOT]
[YoloUNO S3 #3] ─┘                                                                     │
                                                                                       ▼
                                                          [Device1 sensor] ─Rule Chain─▶ RPC ─▶ [Device2 actuator]
```

| Piece | Role |
|---|---|
| ESP32 (YoloUNO) | Reads DHT20, serves web UI, publishes MQTT |
| PC Python broker | Local MQTT server (`tools/tiny_mqtt/tiny_mqtt.py`) |
| PC Python gateway | Forwards local MQTT → CoreIOT cloud |
| CoreIOT | Cloud dashboard + rule chain engine |

### One-time setup

#### 1. Flash ESP32

```bash
make task6
# under the hood:
#   pio run -e task6 -t buildfs
#   pio run -e task6 -t uploadfs
#   pio run -e task6 -t upload
```

If `upload` fails: press RESET once or unplug+replug USB. ESP32-S3 native USB sometimes needs a nudge.

#### 2. Get PC LAN IP

```bash
ip -4 addr | grep inet    # note your IP, e.g. 192.168.1.42
```

#### 3. Configure ESP via its AP

1. Join WiFi `ESP32 LOCAL` (password `12345678`).
2. Browser → `http://192.168.4.1`.
3. Settings tab, fill:
   - SSID/password → **your home WiFi**
   - server → **PC LAN IP** (for local-broker mode) or `app.coreiot.io` (for direct cloud mode)
   - port → `1883`
   - token → device token if going direct; blank if going through gateway
4. Save. ESP reboots, joins your WiFi.
5. Find ESP's new IP from serial monitor.

The firmware auto-detects local vs cloud by checking if `CORE_IOT_SERVER` contains `coreiot.io`. Local mode publishes to topic `gateway/telemetry` with a `deviceName` field; cloud mode publishes to `v1/devices/me/telemetry`.

#### 4. Set up Python tooling (one-time, requires Python 3.8)

```bash
cd tools/tiny_mqtt
uv venv --python 3.8 .venv
uv pip install --python .venv/bin/python -r requirements.txt

cd ../coreiot_test
uv venv --python 3.8 .venv
uv pip install --python .venv/bin/python "paho-mqtt==1.6.1"
```

> **Why Python 3.8**: hbmqtt 0.9.6 breaks on Python 3.9+ (asyncio internals).

### Demo run (5 terminals)

| # | Command | What it shows |
|---|---------|---------------|
| 1 | `make monitor` | ESP serial logs |
| 2 | `cd tools/tiny_mqtt && ./.venv/bin/python tiny_mqtt.py --with-gateway` | Local broker + cloud bridge |
| 3 | `cd tools/coreiot_test && ./.venv/bin/python subscriber.py` | Device 2 actuator (RPC receiver) |
| 4 | `cd tools/coreiot_test && ./.venv/bin/python publisher.py` | Device 1 sensor (temp ramp) |
| 5 | Browser tabs | `http://<ESP_IP>` dashboard, `http://<ESP_IP>/update` OTA, CoreIOT console |

Set `CORE_IOT_GW_TOKEN` in env for terminal 2 (gateway access token from CoreIOT → Entities → Gateways → gear icon). For terminals 3 and 4 set `CORE_IOT_DEV2_TOKEN` and `CORE_IOT_DEV1_TOKEN` respectively.

### What to show the grader

#### Part A — ESP webserver
1. Dashboard shows live temperature + humidity from DHT20.
2. Device tab → add relay with GPIO pin → toggle ON/OFF flips the pin.
3. `/update` page → upload a new `.bin` → wireless firmware update works.

#### Part B — Local MQTT
4. Terminal 2 logs ESP publishing every 10s: `[subscriber] topic=gateway/telemetry payload={"deviceName":"yolo-xxx",...}`.
5. ESP never talks to cloud directly — only the PC does.

#### Part C — Gateway bridge
6. Terminal 2 shows `[gateway] -> yolo-xxx: {'temperature': ...}` forwarding to CoreIOT.
7. CoreIOT Gateway page: `Lab4Gateway` status = Active, your ESP appears as auto-created device with live telemetry.
8. **Talking point**: "N sensors, 1 cloud connection. Gateway pattern."

#### Part D — Rule chain RPC
9. Terminal 4 ramps temp: `26 → 28 → 30 → ... → 60`.
10. Terminal 3 receives RPC automatically:
    - `temp < 30` → `{'method': 'power', 'params': 'off'}`
    - `temp >= 50` → `{'method': 'power', 'params': 'on'}`
11. CoreIOT dashboard button on `Lab4Device2` toggles in sync.
12. **Talking point**: "Logic runs on server, not device. Add a new actuator = tweak rule chain, no firmware reflash."

### Troubleshooting

| Symptom | Fix |
|---|---|
| ESP stuck in AP mode after save | Serial monitor — wrong WiFi pass? `make clean && make task6` |
| Terminal 2 broker crash on Python 3.9+ | Use Python 3.8 (`uv venv --python 3.8 .venv`) |
| CoreIOT gateway shows Inactive | Check `CORE_IOT_GW_TOKEN` matches `Lab4Gateway` token |
| No RPC in Terminal 3 | Rule chain not linked to Device2's profile, or thresholds wrong |
| `upload` fails / native USB | Press RESET once, or unplug + replug USB |

### Reset ESP to AP mode

```bash
pio run -e task6 -t erase
make task6
```

---

## Repo notes

- Multi-env config: each `[env:taskN]` in `platformio.ini` uses `build_src_filter` to only compile that task's sources. The shared `[env]` section sets the board + framework defaults.
- Headers for Task 1 and Task 3 live next to their `.cpp` files (in `src/task1/` and `src/task3/`) — same struct names (`SensorData_t`, etc.) intentionally diverge between tasks, so colocating them avoids `include/` collisions.
- The `coreiot.cpp` is backward-compatible: switches between cloud and local-broker mode based on `CORE_IOT_SERVER` content. Old behavior (point at `coreiot.io`, supply token) still works.
