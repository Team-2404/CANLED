# CANLED
*An open source CAN LED controller for FRC*

![GitHub License](https://img.shields.io/github/license/Team-2404/CANLED)
![GitHub License](https://img.shields.io/badge/FRC-2404-blue?logo=first)


> [!NOTE]
> Picture TBD

## Overview

CANLED is an open source LED controller by FRC team 2404 which communicates over the 
CAN protocol. Based on the ATmega328P microcontroller and MCP2515/2561 CAN 
controller/transciever, this project repository includes the parts (`hardware/docs/`),
schematics (`hardware/kicad/`), firmware (`firmware/canled/`), and software (`software/`) 
needed to assemble the controller and use it with wpilib for FRC. By default set up to control LED strips using the WS2812B controller.

An AVR programmer is needed to flash the firmware using the Arduino IDE.

## Contributors

- @ConorKellyGerakos
- @koshan-dawlaty
- @jakehlee (mentor)
