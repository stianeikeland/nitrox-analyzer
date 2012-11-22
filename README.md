# Nitrox Analyzer

A completely overkill Atmel (AT90USB1287) based nitrox/oxygen analyzer.

Uses a Vandagraph R-17 oxygen micro fuel cell.

## Implemented features:

- Hold current value (push button)
- Easy air calibration (hold button for 1.5 sec)
- Push button to power on, auto power off after 5 minutes idle.
- Nokia 5110 LCD with big oxygen percentage readout + bar graph

## Planned features / Half baked ideas:

- Helium/Trimix analyzer using ultrasound
- Logging of analyzed values w/ date - RTC (+ export via USB).
- Logging of air calibration values (predict fuel cell failure)
- USB Bootloader (for easy firmware upgrades)
- Menu system and rotary encoder for navigation.
- Temperature compensation.

## Schematics:

[![Schematics][schematic-small]][schematic]

## Bill of materials:

- Atmel AT90USB1287
- Vandagraph R-17 Oxygen micro fuel cell
- LTC1779 step down DC/DC Converter
- PCD8544 / Nokia 5110 LCD
- AD8031 Op-amp
- Li-po (or similar power source)
- 8 MHz crystal
- Assortment of resistors, capacitors, push buttons, headers, etc.

## Authors:

- Stian Eikeland
- Bernt Hustad Hembre

## Legal:
The source is released under the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

[schematic]: http://s3.tadkom.net/nitrox/nitrox-schematic.png
[schematic-small]: http://s3.tadkom.net/nitrox/nitrox-schematic-small.png (Schematics)