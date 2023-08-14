# STM32F103xx-Simple-ADC

This project contains the bare minimum code required to get a working ADC example.<br>
<br>
Wiring<br>
LED with 1 K resistor between pin A0 and ground.<br>
10 K potentiometer with outside connections connected to 3V and GND on the Blue Pill. The center tap of the potentiometer goes to pin A1.<br>
<br>
Action<br>
The ADC will read the value from the potentiometer and control the PWM duty cycle of the LED. In other words, the potentiometer can be used to dim or brighten the LED.<br>
<br>
Requires the following for your OS, and the Path variable must be set to point to these binaries:<br>
The makefile will have to be edited as follows: Make sure the FLASHER path variable to the STM32_Programmer_CLI.exe file points to the program in your system, or that the program bin folder is included in your default path. Also, the FLASHPORT variable will have to be updated to to point to the COM port of the attached programmer.
Requires:<br>
<hr>
Editor of your choice<br>
arm-none-eabi-gcc toolchain<br>
make<br>
git-bash (for windows)<br>
STM32_Programmer_CLI.exe Program to upload .elf file to the microcontroller / Blue Pill<br>
<hr>
To compile, navigate to the directory containing main.c (using git-bash in Windows) and type 'make'. Make will also attempt to upload the code to the microcontroller, so, make sure it is connected and ready to accept code before performing the make command. Optionally, the .elf file can be uploaded manually as a separate process.
