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

## Flashing firmware

These instructions assume you are using Windows and a generic USBasp AVR programmer such as [this](https://www.amazon.com/HiLetgo-ATMEGA8-Programmer-USBasp-Cable/dp/B00AX4WQ00?dib=eyJ2IjoiMSJ9.Vpd_TJZkp3x6e6H-i_2vonEUFtTd_0-I6H72u0k_G6s_0to9lSws388s62Q9_aR6_DBn9vL4UkvdUUsjs0d44WYar1vorjVRZY5iNrlMiofNUsc24RplOz-4FCIztb9BsEYEx1MmU4Cnw9Y-3BFWIDUoKXlg4rsoi04-QZFTduGaAsc8hNKOXMywyb7ADZe4hB5U-F0pv_-JWntyFR8pmCsvWxa0H9oXJf_dMJj0bhA.4N6-gxB3QFesddG2YDatRCVYgj06Y0Q0Sk0GaxjhOEg&dib_tag=se&keywords=avr+programmer&qid=1774985295&sr=8-5). 
First, [install zadig.exe](https://zadig.akeo.ie/). Plug in your programmer, connect the CANLED,
and open zadig. Find the USBasp device in the drop-down menu and select it. Choose the WinUSB driver
on the right side using the arrows and click install. Once the driver is installed, open Arduino IDE.
Open up the canled.ino file in the firmware directory (`firmware/canled`) and open up the "tools" 
drop-down menu. Select Arduino Uno as the board and USBasp as the programmer. Open up the "sketch" 
drop-down menu and select "install using programmer". If there is an error, try unplugging and 
replugging the programmer. 


## Contributors

- @ConorKellyGerakos
- @koshan-dawlaty
- @jakehlee (mentor)
