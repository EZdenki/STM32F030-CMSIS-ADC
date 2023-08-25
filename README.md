# STM32F103-CMSIS-ADC-PWM-Example

This project contains the bare minimum code required to demonstrate how to read an ADC input in order to control the blinking rate of an LED, and also
outputs the ADC value out to a serial terminal via the USART if a USB serial dongle is used.<br>
## Wiring<br>
+ Connect an LED with a 1K current limiting resistor between GPIO A5 (pin 11) and ground.<br>
* Connect the outside terminals of a potentiometer (at least 1K) to VCC and GND. The center tap of the potentiometer goes to GPIO A4 (pin 10).<br>
+ The RX line from the USB serial dongle is connected to USART1 TX (pin 8). In order see the serial output, a serial termainal must be used. See
[STM32F030-CMSIS-USART-lib](https://github.com/ezdenki/STM32F030-CMSIS-USART-lib) for details.
