PIO ?= pio

.PHONY: task1 task3 task6 build1 build3 build6 monitor fs clean help

help:
	@echo "Build + flash targets:"
	@echo "  make task1   — Task 1: temperature-driven LED blink"
	@echo "  make task3   — Task 3: LCD temp/humi monitor"
	@echo "  make task6   — Task 6 / Lab 4: full stack (uploads LittleFS first)"
	@echo ""
	@echo "Build-only (no flash):"
	@echo "  make build1 / build3 / build6"
	@echo ""
	@echo "Misc:"
	@echo "  make fs       — re-upload LittleFS for task6 web UI"
	@echo "  make monitor  — open serial monitor (115200)"
	@echo "  make clean    — pio run -t clean"

build1:
	$(PIO) run -e task1

build3:
	$(PIO) run -e task3

build6:
	$(PIO) run -e task6

task1:
	$(PIO) run -e task1 -t upload

task3:
	$(PIO) run -e task3 -t upload

task6:
	$(PIO) run -e task6 -t buildfs
	$(PIO) run -e task6 -t uploadfs
	$(PIO) run -e task6 -t upload

fs:
	$(PIO) run -e task6 -t buildfs
	$(PIO) run -e task6 -t uploadfs

monitor:
	$(PIO) device monitor

clean:
	$(PIO) run -t clean
