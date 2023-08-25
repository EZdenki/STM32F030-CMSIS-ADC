# STM32F030-CMSIS-ADC
This project contains the bare minimum code required to demonstrate how to read an ADC input in order to control the blinking rate of an LED, and also
outputs the ADC value out to a serial terminal via the USART if a USB serial dongle is used.<br>

## Software Setup
The basic steps to set up and read the ADC are as follows. See **main.c** for details.
+ Enable the ADC and GPIO clocks on RCC, and set up the GPIO as an ADC input.
+ Perform calibration of the ADC.
+ Enable the ADC peripheral.
+ Set the ADC channels to be cycled through for reading.
+ Read the ADC value in the main loop of the program.

## Wiring
+ Connect an LED with a 1K current limiting resistor between GPIO A5 (pin 11) and ground.
+ Connect VDDA (pin 5 on the STM32F030F4) to VCC.
+ Connect the outside terminals of a potentiometer (at least 1K) to VCC and GND. The center tap of the potentiometer goes to GPIO&nbsp;A4 (pin&nbsp;10).
+ Connect the RX line from the USB serial dongle to USART1 TX (pin 8), and connect the GND wire of the USB serial dongle to GND on the circuit. In order see the serial output, a serial termainal must be used. See
[STM32F030-CMSIS-USART-lib](https://github.com/ezdenki/STM32F030-CMSIS-USART-lib) for details.
