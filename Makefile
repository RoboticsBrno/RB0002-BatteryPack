.SECONDARY:
export PATH := ./tools/:$(PATH)

BOARDS = $(shell find . -name *.kicad_pcb ! -name _autosave*)

.PHONY: all clean

all: build/charger-gerber.zip build/battery-gerber.zip

build/charger.kicad_pcb: batteryPack/batteryPack.kicad_pcb build
	splitBoard.py $< 0 0 300 100 $@

build/battery.kicad_pcb: batteryPack/batteryPack.kicad_pcb build
	splitBoard.py $< 0 100 300 100 $@

%-gerber: %.kicad_pcb
	exportGerber.py $< $@

%-gerber.zip: %-gerber
	zip -j $@ `find $<`

build:
	mkdir build

clean:
	rm -r build