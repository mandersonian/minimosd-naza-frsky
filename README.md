# minimosd-naza-frsky
Software for Teensy 3.1 that [reads] NAZA GPS serial, [reads] FrSky Smart.Port serial, and [writes] APM MAVLink to a minimOSD 1.1 running stock or lightly-modified software

Goals
Decode and buffer DJI NAZA GPS serial protocol on Teensy 3.1 using existing libraries.
Decode and buffer FrSky? Smart.Port (S.PORT) serial protocol on Teensy 3.1 using existing libraries.
Transmit APM MAVLink serial protocol from Teensy 3.1 to minimOSD 1.1 running stock software.
Additional data sources:
For artificial horizon: NAZA Gimbal Tilt and Roll PWM decoding from F1/F2 ports.
RSSI decoding (low-pass filter and ADC or unfiltered PWM decoding) from FrSky? X8R receiver.
(OPTIONAL) NAZA flight mode decoding from LED port (see minnazaosd for resistor network and ADC pin).
Prior Work

minnazaosd Standalone minimOSD 1.1 running custom software. Replaces MAVLink with NAZA GPS protocol on the hardware UART RX pin (on minimOSD 1.1 6-pin edge connector). Supports PWM decoding of NAZA gimbal outputs (F1/F2 ports) for artificial horizon. Supports analog RSSI decoding (0-1.1V). Requires soldering to tiny Atmega 328p pins for anything other than GPS. I have implemented minnazaosd on a minimOSD 1.1 for the features above (including soldering).

bagaosd Arduino Pro Mini 5V + minimOSD 1.1 running stock or lightly-modified software. Uses the single hardware UART receive pin on the Arduino to decode NAZA GPS serial and uses the SendOnlySoftwareSerial? library to send MAVLink serial protocol to the minimOSD from an Arduino digital pin (pin 8). Since the Pro Mini has only one UART RX/TX, and the RX is used for GPS, there is no hardware UART available to decode the 57600 baud FrSky? S.PORT protocol.

minimosd-extra-frsky by RazvanS Standalone minimOSD 1.1 running custom software. Can decode FrSky? S.PORT protocol directly on the minimOSD, using its single hardware UART. However, the code is not well-documented except for a thread on diydrones.com. The source code needs heavy refactoring, since it re-uses most of the MAVLink variables and source code, replacing only a minimum to receive S.PORT instead of MAVLink on the UART.

PWM/PPM decoding on Teensy 3.1 is a fairly obscure area. The ARM-based MCU has hardware timers and interrupts that are capable of doing this on several I/O pins, but the ISR code is not integrated into any common libraries for Teensy. The Phoenix-FlightController? project has some Teensy 3.1 (Freescale) code for PWM capture. Phoenix-FlightController PWM capture source

Q&A
Q: You already have a DJI NAZA v2. Why not buy an iOSD Mini and spend the extra time flying?
The iOSD Mini is $70, and it can only show what the NAZA knows about. That means it can't do flight battery current (although it can display voltage thanks to the PMU), and it can't do RC receiver RSSI. For about $40 (Teensy 3.1 and minimosd 1.1), you get more OSD data, a lot more ability to customize the OSD layout, and $30 in your pocket. Further, the DJI iOSD Mini is not supported on the NAZA Lite, because it does not have a CAN bus. Being able to have a full-featured OSD with the NAZA Lite also lowers the total cost of the system. NAZA Lite: $170. NAZA v2: $300. (Oct., 2014). NAZA Lite + minimosd-naza-frsky: $210. NAZA v2 + iOSD Mini: $370.
