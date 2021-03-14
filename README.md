paintballfirmware
=================

Paintball Firmware for PIC16f648a microcontrollers. Engineered to work with the Dye Matrix Paintball Gun.

Use [MPLABX](https://www.microchip.com/en-us/development-tools-tools-and-software/mplab-x-ide) to open project files.

Requires the [MPLAB XC8 PRO](https://www.microchip.com/en-us/development-tools-tools-and-software/mplab-xc-compilers) compiler.
The free version of the MPLAB XC8 compiler does not provide enough optimizations for the code to fit on the PIC16f648a flash, unfortunately the PRO version is required (MPLAB offers a 60 day trial).

There are pre-compiled versions of the firmware below.  Early DM4 Matrix
paintball guns came without a low battery indicator (lbi).  The majority of DM4/5
Matrix guns have low battery indicators.

Download Compiled Hex
===

- [lbi DM4/5 firmware](releases/CodeX.X.production.lbi.hex)
- [non-lbi DM4 firmware](releases/CodeX.X.production.nonlbi.hex)

License
=====

paintballfirmware is released under the [MIT License](LICENSE.md)

Author
====

David Whiteside (<david@davidwhiteside.com>)
