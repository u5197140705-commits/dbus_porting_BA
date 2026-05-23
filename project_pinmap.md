# Project Pin Map

Date: 2026-05-22
Project: FRDM-RW612 + Raspberry Pi Pico W + TB6612FNG + LCD1602A + Ultrasonic

## System Overview

1. FRDM-RW612: Zephyr SPI master
2. Raspberry Pi Pico W: SPI slave and peripheral control
3. TB6612FNG motor driver board: dual H-bridge
4. 1602A LCD: HD44780-compatible, 4-bit parallel
5. Ultrasonic sensor board: HC-SR04 style
6. Breadboard/perfboard and jumper wiring

## Parts In Use / Planned

1. DC motors: 2 active in firmware now, target 4 later
2. LEDs: green, yellow, red
3. Resistors: 3 x 220 ohm for LEDs
4. Potentiometer: 10k for LCD contrast
5. Echo voltage divider parts for 3.3V-safe ultrasonic ECHO input
6. USB cables and shared ground wiring

## Board-to-Board SPI (RW612 -> Pico)

1. RW612 GPIO6 / FC1 SSELN0 -> Primary Pico GP17 (CS)
2. RW612 GPIO7 / FC1 SCK -> Pico GP18 (SCK)
3. RW612 GPIO9 / FC1 MOSI -> Pico GP16 (MOSI)
4. RW612 GPIO8 / FC1 MISO <- Pico GP19 (MISO)
5. RW612 GND <-> Pico GND

## RW612 Pin Map (Current Project State)

### SPI / Bus Control

1. GPIO6 -> Primary Pico chip select / FC1 SSELN0
2. GPIO7 -> FC1 SCK
3. GPIO8 -> FC1 MISO
4. GPIO9 -> FC1 MOSI
5. GPIO10 -> Secondary Pico manual CS
6. GPIO18 -> MEXTID3 interrupt input

### Motor0 End Switches

1. GPIO1 -> motor0 end-switch min/home
2. GPIO2 -> motor0 end-switch max/end

Reason for this choice:
1. GPIO1 and GPIO2 are physically broken out on the FRDM-RW612 headers.
2. GPIO11 and GPIO12 are not practical exposed header choices on this board wiring setup.
3. GPIO1 and GPIO2 are not used by the current RW612 SPI/master project code.

Recommended electrical mode:
1. Configure as GPIO input with pull-up
2. Wire each switch between GPIO and GND
3. Logic is active-low
   - pressed = 0
   - released = 1

### Planned Additional End Switches

1. GPIO3 -> motor1 end-switch min/home
2. GPIO4 -> motor1 end-switch max/end
3. GPIO5 -> motor2 end-switch min/home
4. GPIO13 -> motor2 end-switch max/end

Recommended electrical mode:
1. Configure as GPIO input with pull-up
2. Wire each switch between GPIO and GND
3. Logic is active-low
   - pressed = 0
   - released = 1

### RW612 Pins To Avoid For End Switches

1. GPIO6
2. GPIO7
3. GPIO8
4. GPIO9
5. GPIO10
6. GPIO18
7. GPIO11
8. GPIO12

## Planned MISO Hardware Fix

1. Pico1 GP19 should no longer merge directly with Pico2 GP19.
2. Use one tri-state buffer per Pico MISO branch.
3. Connect Pico GP19 -> buffer input, buffer output -> shared MISO node -> RW612 GPIO8.
4. Drive each buffer enable from that Pico's CS line so only the selected Pico is connected to the shared MISO node.

## Pico Pin Map (Current Firmware)

1. GP0 -> LCD RS
2. GP1 -> LCD E
3. GP2 -> LCD D4
4. GP3 -> LCD D5
5. GP4 -> LCD D6
6. GP5 -> LCD D7
7. GP6 -> Green LED
8. GP7 -> Yellow LED
9. GP8 -> Red LED
10. GP9 -> Motor 2 BIN1
11. GP10 -> Motor 2 BIN2
12. GP11 -> Motor 2 PWMB
13. GP12 -> Motor 1 AIN2
14. GP13 -> Motor 1 AIN1
15. GP14 -> TB6612 STBY
16. GP15 -> Motor 1 PWMA
17. GP16 -> SPI MOSI from RW612
18. GP17 -> SPI CS
19. GP18 -> SPI SCK
20. GP19 -> SPI MISO to RW612
21. GP20 -> Ultrasonic TRIG
22. GP21 -> Ultrasonic ECHO via divider

## TB6612FNG Wiring

### Channel A

1. AIN1 = GP13
2. AIN2 = GP12
3. PWMA = GP15

### Channel B

1. BIN1 = GP9
2. BIN2 = GP10
3. PWMB = GP11

### Shared

1. STBY = GP14
2. GND common with Pico and RW612
3. VM and VCC per board requirements

## LCD 1602A Wiring (4-Bit Mode)

1. RS = GP0
2. E = GP1
3. D4 = GP2
4. D5 = GP3
5. D6 = GP4
6. D7 = GP5
7. VSS -> GND
8. VDD -> 5V (VBUS)
9. VO -> potentiometer wiper
10. RW -> GND
11. Backlight wiring per module requirements

## Ultrasonic Sensor Wiring

1. TRIG -> GP20
2. ECHO -> GP21 through voltage divider
3. VCC -> 5V (VBUS)
4. GND -> common GND

## Distance Warning LED Wiring

1. GP6 -> 220 ohm -> Green LED -> GND
2. GP7 -> 220 ohm -> Yellow LED -> GND
3. GP8 -> 220 ohm -> Red LED -> GND
