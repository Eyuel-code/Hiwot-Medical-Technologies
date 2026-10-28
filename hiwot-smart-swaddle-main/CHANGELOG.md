# Changelog

## [Unreleased] - 2026-10
### Added
- `docs/`: final proposal and prototype status report (DOCX + PDF).
- `hardware/schematics/`: Rev B three-sheet schematic (sensors/comms/heater, power/protection, main controller).
- `hardware/pcb/`: Rev A EasyEDA layout preview.
- `hardware/simulation/`: Proteus bench simulation capture.
- `hardware/enclosure/`: FreeCAD lid and base views.
- `hardware/README.md`: revision map and file index.

### Changed
- Power architecture (Rev B): TP4056 + boost converter replaced by 3S Li-ion + BMS,
  fused input, reverse-battery and transient protection, and two TPS62160 regulators (5 V, 3.3 V).
- Schematic split from one sheet into three functional sheets.

### Known gaps
- PCB layout is still Rev A; re-layout against Rev B is pending.
- Source project files (Proteus, EasyEDA, FreeCAD) are not yet committed.
