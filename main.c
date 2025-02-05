/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//**************************
//  MSP430G2xx3 Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  D. Dang
//  Texas Instruments, Inc
//  December 2010
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//**************************

#include <msp430g2553.h>

int main(void) {
  WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

  // DCOCTL = CALDCO_1MHZ;  /* DCOCTL  Calibration Data for 1MHz */
  // BCSCTL1 = CALBC1_1MHZ; /* BCSCTL1 Calibration Data for 1MHz */

  // is this necesary?
  /*
  By default, it's LFXT1S_0 (00) which is 32768 Hz crystal on LFXT1
  */
  // BCSCTL3 &= ~LFXT1S_3; /* Mode 3 for LFXT1 : Digital input signal */

  BCSCTL3 |= LFXT1S_2; /* Mode 2 for LFXT1 : VLOCLK */

  // is this necesary?
  /*
  By default, it's DIVA_0 (00), which is divided by 1
  */
  // BCSCTL1 &= ~DIVA_3; /* ACLK Divider 3: /8 */
  BCSCTL1 |= DIVA_0; /* ACLK Divider 0: /1 */

  // Is this necesary?
  /*
  By default, it's SELM_0 (00), which is DCOCLK
  */
  // BCSCTL2 &= ~SELM_3; /* MCLK Source Select 3: LFXTCLK */
  // BCSCTL2 |= SELM_0; /* MCLK Source Select 0: DCOCLK */

  // Is this necesary?
  /*
  By default, it's DIVM_0 (00), which is divided by 1
  */
  // BCSCTL2 &= ~DIVM_3; /* MCLK Divider 3: /8 */
  // BCSCTL2 |= DIVM_3;  /* MCLK Divider 0: /1 */

  // BCSCTL2 &= ~SELS; /* SMCLK Source Select 0:DCOCLK / 1:XT2CLK/LFXTCLK */

  // Is this necesary?
  /*
  By default, it's DIVS_0 (00), which is divided by 1
  */
  // BCSCTL2 &= ~DIVS_3; /* SMCLK Divider 3: /8 */
  // BCSCTL2 |= DIVS_3;  /* SMCLK Divider 0: /1 */

  // P1.0 = ACLK
  P1DIR |= BIT0;   // P1.0 configured as output
  P1SEL |= BIT0;   // Primary peripheral module function is selected for P1.0
  P1SEL2 &= ~BIT0; // Primary peripheral module function is selected for P1.0

  // P1.6 = TA0.1 -> TA0CCR1
  P1DIR |= BIT6;   // P1.0 configured as output
  P1SEL |= BIT6;   // Primary peripheral module function is selected for P1.6
  P1SEL2 &= ~BIT6; // Primary peripheral module function is selected for P1.6

  TA0CTL &= ~MC_0;      /* Timer A mode control: 0 - Stop */
  TA0CTL |= TASSEL_1;   /* Timer A clock source select: 1 - ACLK  */
  TA0CTL |= ID_0;       /* Timer A input divider: 0 - /1 */
  TA0CCTL1 |= OUTMOD_7; /* PWM output mode: 7 - PWM reset/set */
  TA0CCR0 = 11 - 1;
  TA0CCR1 = 8;
  TA0CTL |= MC_1; /* Timer A mode control: 1 - Up to CCR0 */

  __bis_SR_register(LPM3_bits | GIE);
}
