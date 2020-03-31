# RB0002-BatteryPack

Universal Li-Ion battery pack for robots.

# Repository Structure

- `batteryPack` contains the main schematics and board drawing
- `footprints` contains the board-specific footprints
- `symbols` contains the board-specific symbols
- `tools` contains few scripts for automatization

# Building

Simply call `make` in the top-top level directory. The `build` directory will
them contain:

- split boards for the charger and the battery pack
- zipped gerbers for the boards you can directly use for manufacturing
