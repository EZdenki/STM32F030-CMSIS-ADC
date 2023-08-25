//  ==========================================================================================
//  STM32F030-CMSIS-USART-lib.c
//  ------------------------------------------------------------------------------------------
//  Minimalist Serial UART library for the STM32F030
//  ------------------------------------------------------------------------------------------
//  https://github.com/EZdenki/STM32F030-CMSIS-USART-lib
//  Released under the MIT License
//  Copyright (c) 2023
//  Mike Shegedin, EZdenki.com
//  Version 1.1   16 Aug 2023   Added baud rate as parameter to USART_init.
//  Version 1.0   24 Jul 2023   Updated core files and comments
//  ------------------------------------------------------------------------------------------
//  Target Devices: STM32F030xx and a USB-Serial dongle
//
//  Hardware: STM32030xx, USB-Serial dongle
//  Software: PuTTY for Windows or Linux (or any other serial terminal program)
//
//  ------------------------------------------------------------------------------------------
//  Summary:
//    Library of most basic functions to support serial communication to and from the
//    STM32F030. Note that only USART1 is currently supported.
//
//    USART1_Tx = PA2 (pin 8), Alternate Function 1
//    USART1_Rx = PA3 (pin 9), Alternate Function 1
//
//    Baudrate Calculation:
//      Assuming the internal RC clock: f(CK) = 8 MHz
//        Mantissa = whole part of f(CK) / (16 * Baud)
//        Fraction = remainder of above * 16
//          f(CK)    Baud     Mantissa   Fraction
//          -----   -------   --------   --------
//          8 MHz     9,600      52          1
//                  115,200       4          5
//                  460,800       1          1
//                  500,000       1          0
//
//        If calculating in program:
//          uartDiv  = freqClk / baud
//          mantissa = uartDiv / 16
//          fraction = uartDiv % 16
//
//    Steps to Set Up UART on STM32F030xx:
//      1. Enable GPIO Port A via RCC->AHBENR
//      2. Set PA2 and PA3 as Alternate Functions via GPIOA->MODER
//      3. Set Alternate Function 1 for PA2 and PA3 via GPIOA->AFR[0]
//      4. Enable USART1 peripheral via RCC->APB2ENR
//      5. Set Baudrate via USART1->BRR
//      6. Enable (turn on) Tx, Rx, and USART via USART1->CR1
//  ==========================================================================================

#ifndef __STM32F030_CMSIS_USART_LIB_C
#define __STM32F030_CMSIS_USART_LIB_C

#include "stm32f030x6.h"  // Primary CMSIS header file

// void
// USART_putc
// Output a single character to the USART Tx pin (PA2)
void
USART_putc( char c )
{
    while( !(USART1->ISR & USART_ISR_TXE ) )
    {   // Wait until the transmit data register is empty
    }
    USART1->TDR = c; // Put character into the data register.
}


// void
// USART_puts
// Output a string to the USART Tx pin (PA2). Adds linefeed after string.
// String should be null terminated (standard C string)
void
USART_puts( char *s )
{
    while( *s )
        USART_putc( *s++ );
     USART_putc('\n');
}


// char
// USART_getc( void )
// Waits for a character on the serial port and returns the character
// when received.
char
USART_getc( void )
{
    while( !( USART1->ISR & USART_ISR_RXNE ) )
    {
    }
    return USART1->RDR;
}


// void
// USART_init( void )
// Initializes USART port and Tx/Rx pins and readies the port to receive and transmit data
// at the specified baud rate. Possible baud rates are from 300 to 460,800.
void
USART_init( uint32_t baudrate )
{
    uint32_t speedMant, speedFrac;
    
    // Calculate the mantissa (speedMant) and fraction (speedFrac) values for an 8 MHz CPU
    // Note that the 8E6 constants are cast as uint32_t, otherwise gcc will consider them
    // to be floating constants with much more overhead.
    speedMant  = (uint32_t)8E6 / baudrate / 16;
    speedFrac = ( (uint32_t)8E6 - baudrate * speedMant * 16 ) / baudrate;


    // Enable GPIO Port A
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Set PA2 and PA3 as Alternate Functions
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODER2_Pos); // USART1_TX/PA2/AF1/Pin8
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODER3_Pos); // USART1_RX/PA3/AF1/Pin9
    
    // Set PA2 and PA3 as Alternate Function 1
    GPIOA->AFR[0] |= (0b0001 << GPIO_AFRL_AFRL2_Pos);
    GPIOA->AFR[0] |= (0b0001 << GPIO_AFRL_AFRL3_Pos);
    
    // Enable USART1 peripheral
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    
    // Set Baudrate by loading the baudrate Mantissa and Fractional part as described above
    USART1->BRR = ( speedMant << USART_BRR_DIV_MANTISSA_Pos ) |
                  ( speedFrac << USART_BRR_DIV_FRACTION_Pos );
    
    // Enable (turn on) Tx, Rx, and USART
    USART1->CR1 = (USART_CR1_TE | USART_CR1_RE | USART_CR1_UE) ;
    
    // End of Setup
}

#endif /* __STM32F030-CMSS_USART_LIB_C   */
