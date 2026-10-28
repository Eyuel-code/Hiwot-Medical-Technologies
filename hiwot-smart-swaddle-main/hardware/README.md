# Hiwot Hardware

Design artifacts for the **Hiwot Smart Swaddle & Connected Neonatal Mesh Pod**
(INSA Summer Bootcamp Program 2026). The authoritative project description is
[`docs/Hiwot_Proposal_Final.pdf`](../docs/Hiwot_Proposal_Final.pdf).

## Revision map

| Revision | Date range | What it is |
|---|---|---|
| **Rev A** | 2026-08-29 | Single-sheet EasyEDA schematic, first PCB layout, Proteus bench simulation. Matches the status report in the proposal (Section 21). |
| **Rev B** | 2026-09-08 → 2026-09-22 | Schematic split into three functional sheets, with a new 3S Li-ion power tree. **The PCB has not yet been re-laid-out against Rev B.** |

> The PCB layout in this repository is **Rev A** and does not match the Rev B
> schematics. Treat Rev B schematics as the current design intent.

## Schematics (Rev B, EasyEDA)

| Sheet | File | Contents |
|---|---|---|
| 1 | [`sensors-comms-heater`](schematics/rev-b_sheet1_sensors-comms-heater_2026-09-22.png) | ESP8266 (ESP-01S) Wi-Fi link, reset/enable network, dual NTC temperature sensing (baby / environment), Grove piezo respiration sensor, LM358 signal conditioning, PTC heater driver (IRLZ44N low-side MOSFET, 5 A thermal fuse) |
| 2 | [`power-battery-protection`](schematics/rev-b_sheet2_power-battery-protection_2026-09-08.png) | 3S Li-ion pack + BMS, 7.5 A main fuse, TVS and bulk/bypass capacitance, P-MOSFET reverse-battery protection, 5 V and 3.3 V TPS62160 buck regulators, separate heater-branch fuse |
| 3 | [`main-controller`](schematics/rev-b_sheet3_main-controller_2026-09-21.png) | ATmega328P, reset circuit, ISP header, 16 MHz crystal, I²C 1602 LCD, buzzer driver, red/green status LEDs |

Rev A (superseded): [`rev-a_monolithic`](schematics/rev-a_monolithic_2026-08-29.png)

## PCB

- [`rev-a_pcb-layout_easyeda`](pcb/rev-a_pcb-layout_easyeda_2026-08-29.png): EasyEDA top/bottom copper routing, Rev A.

## Simulation

- [`proteus_bench-v0`](simulation/proteus_bench-v0_2026-08-29.png): Proteus 8 capture of the breathing-check front end, temperature sensor, opto-isolated PWM MOSFET heater driver, ESP8266, LCD and alarm stage.

## Enclosure (FreeCAD)

| View | File |
|---|---|
| Lid, top view (LED windows, port hole, four screw holes) | [`lid_top-view_freecad`](enclosure/lid_top-view_freecad.png) |
| Base interior (corner posts and PCB standoffs) | [`base_interior-standoffs_freecad`](enclosure/base_interior-standoffs_freecad.png) |

## Naming convention

`<revision>_<what>_<tool>_<YYYY-MM-DD>.<ext>`. Source files (`.pdsprj`, EasyEDA
project JSON, Gerbers, `.FCStd`, STEP/STL) go in the same folder as their
preview image, using the same base name.
