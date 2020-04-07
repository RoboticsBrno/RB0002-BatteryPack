.PHONY: all clean

BOARDS = charger battery connector battery-panel charger-panel connector-panel
GERBERS = $(addprefix build/, $(BOARDS:=-gerber.zip))

all: $(GERBERS)

build/charger.kicad_pcb: batteryPack/batteryPack.kicad_pcb build
	kikit panelize extractboard -s 175 50 60 40 $< $@

build/battery.kicad_pcb: batteryPack/batteryPack.kicad_pcb build
	kikit panelize extractboard -s 20 20 70 90 $< $@

build/connector.kicad_pcb: batteryPack/batteryPack.kicad_pcb build
	kikit panelize extractboard -s 116 52 30 36 $< $@

build/battery-panel.kicad_pcb: build/battery.kicad_pcb
	kikit panelize grid --space 3 --gridsize 1 2 --tabwidth 30 --tabheight 60 --vcuts 1 --radius 1 $< $@

build/connector-panel.kicad_pcb: build/connector.kicad_pcb
	kikit panelize grid --space 3 --gridsize 2 4 --tabwidth 13 --tabheight 28 --vcuts 1 --radius 1 $< $@

build/charger-panel.kicad_pcb: build/charger.kicad_pcb
	kikit panelize grid --sourcearea 115 75 70 50 --space 3 --gridsize 2 2 --tabwidth 40 --tabheight 28 --vcuts 1 --radius 1 $< $@


%-gerber: %.kicad_pcb
	kikit export gerber $< $@

%-gerber.zip: %-gerber
	zip -j $@ `find $<`

build:
	mkdir -p build

clean:
	rm -r build