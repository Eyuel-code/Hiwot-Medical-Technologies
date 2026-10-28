# Hiwot Smart Swaddle 

**Hiwot Medical Technologies**: a low-cost, fail-safe heated smart swaddle with breathing (apnea) monitoring and a nursing-station dashboard for preterm infants in Ethiopian hospitals.
INSA Summer Bootcamp 2026 project, 8-person student team.

> **Status: first prototype in progress (August 2026).** This repo holds what exists today. Items marked *planned* are not built yet.

## What the first prototype is
- **ATmega328P safety core**: reads skin/ambient NTC thermistors, drives a PTC heater through a MOSFET (PWM), and detects apnea from a piezo respiration sensor with a threshold-based detector. This works with no network (the offline track).
- **ESP32 telemetry bridge** *(planned for this prototype)*: forwards readings as JSON over local WiFi to a laptop backend.
- **Backend AI** *(in progress, ~45%)*: Python FastAPI service plus a 1D-CNN trained on PhysioNet PICSDB.
- **Nursing dashboard**: React front end done; backend wiring in progress (localhost).
- The original target design (dual MCU, on-device TinyML, BLE/ESP-NOW mesh) is described in the proposal; the above is the first-prototype scope only.

## Repository layout
| Folder | Contents |
|---|---|
| `firmware/` | ATmega328P C firmware (`src/`) and `Makefile` |
| `hardware/proteus/` | Proteus simulation capture |
| `hardware/pcb/` | EasyEDA schematic and PCB layout captures |
| `hardware/cad/` | Enclosure CAD captures (FreeCAD) |
| `ai/` | Baseline model script, requirements, AI progress report |
| `dashboard/` | Nursing station dashboard capture |
| `docs/` | Full proposal and status report; `concept-renders/` are AI-generated concept art, not photos of the build |

## Build the firmware
```bash
sudo apt install gcc-avr avr-libc avrdude
cd firmware
cp src/secrets.h.example src/secrets.h   # then edit with your own WiFi/API values
make            # builds hiwot_firmware.hex
make flash      # flashes with a USBasp programmer
```
`secrets.h` is git-ignored. Never commit WiFi passwords or API keys.

## Run the AI baseline
```bash
cd ai
python -m venv venv && source venv/bin/activate
pip install -r requirements.txt
# download PICSDB from PhysioNet into this folder, then:
python train_model.py
```

## Known limitations (honest status)
- The committed firmware is the earlier bench iteration (ESP8266 AT commands to ThingSpeak); migration to the ESP32 + local backend path is pending.
- Thermal control is proportional/bang-bang, not full PID; no watchdog timer yet.
- `train_model.py` uses placeholder all-zero labels; real labels and training are the next milestone.
- The custom PTC heating grid is not yet fabricated (no ready-made PTC blanket is sold in Ethiopia; imported from a trusted vendor is the production plan).
- Not a certified medical device. Bench and non-clinical validation only.

## Team (alphabetical)
Beemnet Girum, Betelhem Tadesse, Bethel Yohanes, Dawit Mulugeta, Etsub Dessalegn, Eyuel Wondafrash, Musab Mohammedamin, Yonas Solomon. This was a collaborative build; roles are in the proposal (Section 13).

## Hardware

| | |
|---|---|
| Controller | ATmega328P (thermal control, apnea detection, LCD) |
| Connectivity | ESP8266 (ESP-01S) telemetry bridge |
| Sensing | Dual NTC (baby / environment), piezo respiration sensor with LM358 conditioning |
| Heating | PTC heater, PWM-driven IRLZ44N, independent 5 A thermal fuse |
| Power | 3S Li-ion + BMS, fused, reverse-protected, TPS62160 5 V / 3.3 V rails |

![Main controller](hardware/schematics/rev-b_sheet3_main-controller_2026-09-21.png)

Full schematics, PCB, simulation and enclosure files: [`hardware/`](hardware/README.md).
Project proposal and status report: [`docs/Hiwot_Proposal_Final.pdf`](docs/Hiwot_Proposal_Final.pdf).
Project proposal and status report: [`docs/Hiwot_Medical_Technologies.pdf`](docs/Hiwot_Medical_Technologies.pdf)
