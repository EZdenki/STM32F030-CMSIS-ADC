# STM32F103-CMSIS-ADC-PWM-Example

This project contains the bare minimum code required to demonstrate how to read an ADC input in order to control the brightness of an LED via PWM.<br>
<br>
## Wiring<br>
LED with 1 K resistor between pin A0 and ground.<br>
10 K potentiometer with outside connections connected to 3V and GND on the Blue Pill. The center tap of the potentiometer goes to pin A1.<br>
<br>
## Action<br>
The ADC will read the value from the potentiometer and control the PWM duty cycle of the LED. In other words, the potentiometer can be used to dim or brighten the LED.
