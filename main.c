//  ==========================================================================================
//  main.c for STM32F103-CMSIS-ADC
//  ------------------------------------------------------------------------------------------
//  Demostrates how to set up simple polling ADC conversion on the STM32F103F4xxx.
//  Set up ADC input on GPIO pin A4 (pin 10) that reads a potentiometer voltage divider.
//  The output is used to control the blinking rate of an LED. And the value read from the ADC
//  is also output to the USART TX pin (pin 8). The value can be read on a serial terminal by
//  hooking up a USB serial dongle and running a serial program such as PuTTY. See the file,
//  STM32F030-CMSIS-USART-lib.c or https://github.com/EZdenki/STM32F030-CMSIS-USART-lib for
//  details.
//  ------------------------------------------------------------------------------------------
//  https://github.com/EZdenki/STM32F030-CMSIS-ADC-Test
//  Released under the MIT License
//  Copyright (c) 2023
//  Mike Shegedin, EZdenki.com
//
//    Version 1.0   25 Aug 2023   Added serial USART output
//    Version 0.9   24 Aug 2023   Started port from STM32F103-CMSIS-ADC-PWM-Example
//
//  Target Devices:
//    STM32F030F4xx
//    1K ohm or higher potentiometer (pot)
//    LED and 1k ohm current limiting resistor
//    Optional: USB Serial Dongle
//
//  ------------------------------------------------------------------------------------------
//  Hardware Setup
//
//                                          STM32F030F4xx
//                                           .---. .---.
//                                     BOOT0 |1o  V  20| PA14 / SWCLK
//                                    OSC_IN |2      19| PA13 / SWDIO
//                                   OSC_OUT |3      18| PA10
//          GND -- [10K] -- [BUTTON] -- NRST |4      17| PA9
//          VCC ----------------------- VDDA |5      16| VCC --------------------- VCC
//                                       PA0 |6      15| GND --------------------- GND
//                                       PA1 |7      14| PB1
//  Serial Dongle RX Line -- USART1 TX / PA2 |8      13| PA7
//                                       PA3 |9      12| PA
//                        ,--- ADC_IN4 / PA4 |10     11| PA5 -- [+LED-] -- [1K] -- GND
//                        |                  '---------'
//                        |
//                        |                  ,-----,
//                        |         VCC --- o|     |  |]
//                        '---------------- o| POT |==|] 
//                                  GND --- o|     |  |]
//                                           '-----'
//                                          
//
//  ==========================================================================================

#include "stm32f030x6.h"
#include "STM32F030-CMSIS-USART-lib.c"
#include <stdlib.h>
int
main( void )
{

  USART_init( 115200 );                 // Connect to USART1 (PA2 / pin 8) at this baud rate
  USART_puts( "USART Connected!\n" );   // Show message to serial terminal

  // -----------------------------------------------------------------------------------------
  // Enable RCC timer for GPIOA, and ADC, and set up GPIO MODER bits
  // -----------------------------------------------------------------------------------------
  RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

  // Set up GPIO Pin A5 as an output for the LED. Do this by setting the MODER5[1:0] bits
  // to 0b01.
  GPIOA->MODER |= ( 0b01 << GPIO_MODER_MODER5_Pos );
                                        
  // Set up GPIO Pin A4 as an ADC input. Do this by setting the MODER4[1:0] bits to 0b11.
  GPIOA->MODER |= ( 0b11 << GPIO_MODER_MODER4_Pos );


  // -----------------------------------------------------------------------------------------
  // Calibrate ADC (Following Apendix A.7.1 in Reference Manual RM0360)
  // -----------------------------------------------------------------------------------------
  
  // (1) Ensure that ADEN = 0
  // (2) Clear ADEN by setting ADDIS
  // (3) Clear DMAEN
  // (4) Launch the calibration by setting ADCAL
  // (5) Wait until ADCAL=0

  // Make sure the ADC is not enabled, and if it is, then disable it.
  // Note that the ADC can only be enabled by setting the ADC_CR_ADEN bit,
  // and disabled by setting the ADC_CR_ADEN bit.
  if ((ADC1->CR & ADC_CR_ADEN) != 0)        // If ADC enabled,
    ADC1->CR |= ADC_CR_ADDIS;               // then disable it.

  while ((ADC1->CR & ADC_CR_ADEN) != 0) ;   // Wait for the CR_ADEN bit to be cleared.

  ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;          // Disable DMA
  ADC1->CR |= ADC_CR_ADCAL;                 // Set ADC_CR_ADCAL bit to start calibration
  while ((ADC1->CR & ADC_CR_ADCAL) != 0) ;  // Wait for bit to be cleared, which indicates
                                            // calibration is complete.

  // -----------------------------------------------------------------------------------------
  // Enable ADC (Following Appendix A.7.2 in Reference Manual RM0360)
  // -----------------------------------------------------------------------------------------
  
  // (1) Ensure that ADRDY = 0
  // (2) Clear ADRDY
  // (3) Enable the ADC
  // (4) Wait until ADC ready
  
  if(( ADC1->ISR & ADC_ISR_ADRDY ) != 0 )     // If ADC previously enabled and is ready,
    ADC1->ISR |= ADC_ISR_ADRDY;               // then clear it by setting the bit.

  ADC1->CR |= ADC_CR_ADEN;                    // (Re)enable the ADC
  while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) ;  // Wait for the ADC to be enabled

 
  // -----------------------------------------------------------------------------------------
  // Single conversion sequence code example - Software trigger
  // (Following Appendix A.7.6 in Reference Manual RM0360)
  // -----------------------------------------------------------------------------------------
  // (1) Select HSI14 by writing 00 in CKMODE (Default value, therefore not needed)
  // (2) Select CHSEL4 for VRefInt
  // (3) Select a sampling mode of 111 i.e. 239.5 ADC clk to be greater than 17.1 us
  // (4) Wake-up the VREFINT (only for VBAT, Temp sensor and VRefInt) (not needed)

  ADC1->CHSELR  =  ADC_CHSELR_CHSEL4;           // Select ADC channel 4 for conversion
  ADC1->SMPR   |=  (0b111 << ADC_SMPR_SMP_Pos); // Select longest ADC sampling time


  // -----------------------------------------------------------------------------------------
  // Endless Loop
  // -----------------------------------------------------------------------------------------
  char istring[10];

  while( 1 )
  {
    GPIOA->ODR ^= GPIO_ODR_5;                       // Toggle LED

    // The following 3 lines are needed for every conversion:
    ADC1->CR |= ADC_CR_ADSTART;                     // Initialize the ADC conversion
    while(( ADC1->ISR & ADC_ISR_EOC ) == 0 ) ;      // Wait for the end of the conversion
    uint16_t ADC_Result = ADC1->DR;                 // Get the result

    itoa( ADC_Result, istring, 10 );                // Convert ADC value to a string
    USART_puts( istring );                          // Output the string to the terminal

    for( uint32_t x=0 ; x<(ADC_Result*100); x++) ;  // Pause according to the result of
                                                    // the returned ADC value.
  }

} // End main.c
