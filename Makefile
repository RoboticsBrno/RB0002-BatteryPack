.PHONY: all clean web

BOARDS = charger battery connector battery-panel charger-panel connector-panel
BOARDSFILES = $(addprefix build/, $(BOARDS:=.kicad_pcb))
GERBERS = $(addprefix build/, $(BOARDS:=-gerber.zip))

RADIUS=1

all: $(GERBERS) build/web/index.html

build/charger.kicad_pcb: batteryPack/batteryPack.kicad_pcb build
	kikit panelize extractboard -s 175 50 70 40 $< $@

build/battery.kicad_pcb: batteryPack/batteryPack.kicad_pcb build
	kikit panelize extractboard -s 20 20 70 90 $< $@

build/connector.kicad_pcb: batteryPack/batteryPack.kicad_pcb build
	kikit panelize extractboard -s 116 52 30 36 $< $@

build/battery-panel.kicad_pcb: build/battery.kicad_pcb
	kikit panelize grid --space 3 --gridsize 1 2 --tabwidth 30 --tabheight 60 \
		--vcuts --radius $(RADIUS) $< $@

build/connector-panel.kicad_pcb: build/connector.kicad_pcb
	kikit panelize grid --space 3 --gridsize 2 4 --tabwidth 10 --tabheight 24 \
		--vcuts --radius $(RADIUS) $< $@

build/charger-panel.kicad_pcb: build/charger.kicad_pcb
	kikit panelize grid --sourcearea 115 75 70 50 --space 3 --gridsize 2 2 \
		--tabwidth 40 --tabheight 28 --vcuts --radius $(RADIUS) $< $@


%-gerber: %.kicad_pcb
	kikit export gerber $< $@

%-gerber.zip: %-gerber
	zip -j $@ `find $<`

web: build/web/index.html

build:
	mkdir -p build

build/web: build
	mkdir -p build/web

build/web/index.html: build/web $(BOARDSFILES)
	kikit present boardpage \
		-d README.md \
		--name "Battery Pack" \
		-b "Charger Board" " " build/charger.kicad_pcb  \
		-b "Battery Board" " " build/battery.kicad_pcb  \
		-b "Connector Board" " " build/connector.kicad_pcb  \
		-b "Charger Board Panelized" " " build/charger-panel.kicad_pcb  \
		-b "Battery Board Panelized" " " build/battery-panel.kicad_pcb  \
		-b "Connector Board Panelized" " " build/connector-panel.kicad_pcb  \
		--repository 'https://github.com/RoboticsBrno/RB0002-BatteryPack' \
		build/web



clean:
	rm -r build