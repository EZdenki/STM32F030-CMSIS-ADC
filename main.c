//  main.c for STM32F103-CMSIS-ADC-test
//
//  Minimalist polling ADC conversion example. Also demostrates basic PWM output.
//  Mike Shegedin, EZdenki.com
//
//      Version 1.0     14 Aug 2023   Updated core files, cleaned up comments
//      Version 0.9        Mar 2023   Started
//
//  Set up ADC input on Pin A1 that reads a potentiometer voltage divider.
//  The output is used to control the duty cycle of a 10 kHz PWM signal output
//  generated on Timer 2, Channel 1, Pin A0.


//  Steps to Set Up PWM:
//  1. Enable TIMER 2 by setting the RCC_APB1ENR_TIM2EN bit in the RCC APB1ENR register.
//  2. Enable GPIO Port A by setting the RCC_APB2ENR_IOPAEN bit in the RCC APB2ENR register.
//  3. Set CNF and MODE bits for GPIO Port A, Pin 0 for output, push-pull, alternate function,
//     2 MHz speed, by setting the appropriate CNF0 and MODE0 bits in the GPIOA CRL register.
//  4. Set PWM period via the TIM2 ARR register.
//  5. Set PWM duty cycle via the TIM2 CCR1 register.
//  6. Set PWM mode 1 by setting the TIM_CCMR1_OC1Mx bits as 110 via the TIM2 CCMR1 register.
//  7. Enable output on Channel 1 of Timer 2 by setting the TIM_CCER_CC1E bit in the TIM2 CCER
//     register.
//  8. Enable the clock counter via the TIM_CR1_CEN bit in the TIM2 CR1 register.

#include "stm32f103xb.h"

int
main( void )
{

  // =========================================
  // Setup up PWM Output on GPIO A0 (TIM2/CH1)
  // =========================================


  // ====================
  // Set up RCC Registers
  // ====================
  // Enable TIM2
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // Enable GPIO Port A, ADC1
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;

  // =====================
  // Set up GPIO Registers
  //    PWM Output: A0
  //    ADC Input:  A1
  // =====================
  // Set the CNF and MODE bits for Pin A0 as follows:
  // Set CNF0[1:0] to 0b10 for Alternate function output, push-pull. Alternate function as
  // defined by the CNF bits is needed for PWM functionality.
  // Set MODE0[1:0] to 0b10 for output functionality with a max speed of 2 MHz.
  // Note that CNF[1] bit is set to 1 by default, and that bit must be cleared!
  GPIOA->CRL &= ~GPIO_CRL_CNF0_Msk;                       // Clear both CNF bits
  GPIOA->CRL |= ( GPIO_CRL_CNF0_1 | GPIO_CRL_MODE0_1 );   // Set CNF0[1] and MODE0[1] bits.

  // Clear CNF1[1:0] bits for analog input on pin A1
  GPIOA->CRL &= ~GPIO_CRL_CNF1_Msk;


  // ====================
  // Set up ADC Registers
  //    ADC Channel: Ch2 / GPIO A1
  // ====================

  // Set up Channel


  // Set Ch2 (GPIO A1) as first and only conversion
  // Set SQ1[4:0] to 1 (0b00001)
  ADC1->SQR3 |= ( 1U << ADC_SQR3_SQ1_Pos );

  // The following line not needed since the default L is 1
  // ADC1->SQR1 |= ( 0b0000 << ADC_SQR1_L_Pos );

  // Set ADC Sample Time to 7.5 cycles. This seems to perform much
  // better than the reset value of 1.5 cycles.
  ADC1->SMPR2 |= ( 0b001 << ADC_SMPR2_SMP1_Pos );

  // Enable ADC1 first time (standby until first conversion)
  ADC1->CR2 |= ADC_CR2_ADON;

  for( uint32_t x=0; x<7; x++) ;

  // Initialize calibration registers
  ADC1->CR2 |= ADC_CR2_RSTCAL;
  while( ADC1->CR2 & ADC_CR2_RSTCAL ) ; // Wait until registers are initialized.

  // Perform ADC Calibration
  ADC1->CR2 |= ADC_CR2_CAL;
  while ( ADC1->CR2 & ADC_CR2_CAL ) ;   // Wait until calibration is finished.


  // ======================
  // Set up Timer Registers
  // ======================
  // Set TIM2 Frequency to 4096, which is the ADC resolution + 1.
  // The value read in by the ADC will determine the duty cycle, and
  // will be loaded into TIM2->CCR1.
  TIM2->ARR = 4096;

  // Set TIM2/CH3 to PWM Mode 1 (OC1M[2:0] = 0b110)
  TIM2->CCMR1 |=  ( TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 );

  // Enable output on Timer 2 Channel 1 pin
  TIM2->CCER |= TIM_CCER_CC1E;

  // Finally, enable the clock counter (turn on the clock!)
  TIM2->CR1 |= TIM_CR1_CEN;

  // Endless Loop
  while( 1 )
  {
    ADC1->CR2 |= ADC_CR2_ADON;            // Start ADC read...
    for( uint32_t x=0 ; x<7; x++) ;       // Wait for approx. one conversion length
    
    while( !(ADC1->SR & ADC_SR_EOC )) ;   // Wait until conversion is complete.
    
    TIM2->CCR1 = (ADC1->DR & 0xFFFF)+1;   // Set TIM2 duty cycle to the ADC value + 1.
  }

} // End main.c
