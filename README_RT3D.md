<!-- omit in toc -->
# Modification of the Fork JimmymaDdness/Mks-TouchScreenFirmware-Duet firmware for stabilizing and optimizing 
### Tested with firmware [RepRapFirmware 3.1.1 Port for LPC (3.1.1-14)](https://github.com/gloomyandy/RepRapFirmware/releases/tag/LPC_RRFv3.1.1-14)

## General information
You can find an installation video in German on [Youtube](https://youtu.be/RYbedIfLLbQ)

## Stabilizing
For stabilizing the firmware, I changed folowing parts:
  - changed acces to the RepRapFirmware filesystem.
  - Menu print work now stable
  - implement a timeout for temerature status

## Optimizing
  - I changed the Custom menu to "Macros". So I yan list the Macros and start them from the display.
  - Implement a watchdoc. Display restart after 20s hangup
  - Display Errors and Warnings on Statusscreen
