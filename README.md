# RB0002-BatteryPack

Universal 2 cells Li-Ion battery pack with separate USB charger PCB for robot power supplying.

This battery pack is designed for continuous 8.4 V / 15 A.

View this project on [CADLAB.io](https://cadlab.io/project/22740/).

## Repository Structure

- `batteryPack` contains the main schematics and board drawing
- `doc` contains CubeMX pinout of the MCU, resistor calculation spreadsheet, etc.
- `footprints` contains the board-specific footprints
- `symbols` contains the board-specific symbols
- `tools` contains few scripts for automatization

## Building

Simply call `make` in the top-top level directory. The `build` directory will
them contain:

- split boards for the charger and the battery pack
- zipped gerbers for the boards you can directly use for manufacturing
