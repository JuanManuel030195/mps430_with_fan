#include <msp430g2553.h>

/*
Unused I/O pins should be configured as I/O function, output direction, and left
unconnected on the PC board, to prevent a floating input and reduce power
consumption. The value of the PxOUT bit is irrelevant, since the pin is
unconnected. Alternatively, the integrated pullup/pulldown resistor can be
enabled by setting the PxREN bit of the unused pin to prevent the floating
input.
*/

#define NO_SPEED_VALUE 0
#define LOW_SPEED_VALUE 4
#define MID_SPEED_VALUE 7
#define FULL_SPEED_VALUE 10

#define ENABLE_PUSH_BUTTON BIT7
#define SPEED_PUSH_BUTTON BIT3
#define SPEED_OUTPUT BIT6

#define LOW_SPEED_LED BIT3
#define MID_SPEED_LED BIT1
#define FULL_SPEED_LED BIT5

int main(void) {
  WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

  BCSCTL3 |= LFXT1S_2; /* Mode 2 for LFXT1 : VLOCLK 12KHz*/
  BCSCTL1 |= DIVA_0;   /* ACLK Divider 0: /1 */

  // P1.3 = Speed Push Button
  P1DIR &= ~SPEED_PUSH_BUTTON;  // P1.3 configured as input
  P1REN |= SPEED_PUSH_BUTTON;   // P1.3 pullup or pulldown resistor enabled
  P1OUT |= SPEED_PUSH_BUTTON;   // P1.3 is pulled up
  P1SEL &= ~SPEED_PUSH_BUTTON;  // I/O function on P1.3
  P1SEL2 &= ~SPEED_PUSH_BUTTON; // I/O function on P1.3
  P1IES |= SPEED_PUSH_BUTTON;   // P1IFG3 flag set with a high-to-low transition
  P1IFG &= ~SPEED_PUSH_BUTTON;  // P1IFG3 reset with software
  P1IE |= SPEED_PUSH_BUTTON;    // Interrup enabled for P1.3

  // P2.7 = Enable Push Button
  P2DIR &= ~ENABLE_PUSH_BUTTON;  // P2.7 configured as input
  P2REN |= ENABLE_PUSH_BUTTON;   // P2.7 pullup or pulldown resistor enabled
  P2OUT |= ENABLE_PUSH_BUTTON;   // P2.7 is pulled up
  P2SEL &= ~ENABLE_PUSH_BUTTON;  // I/O function on P2.7
  P2SEL2 &= ~ENABLE_PUSH_BUTTON; // I/O function on P2.7
  P2IES |= ENABLE_PUSH_BUTTON;  // P2IFG7 flag set with a high-to-low transition
  P2IFG &= ~ENABLE_PUSH_BUTTON; // P2IFG7 reset with software
  P2IE |= ENABLE_PUSH_BUTTON;   // Interrup enabled for P2.7

  // P2.3, P2.1, P2.5 = SPEED LEDS
  P2DIR |= LOW_SPEED_LED | MID_SPEED_LED | FULL_SPEED_LED;
  P2OUT &= ~(LOW_SPEED_LED | MID_SPEED_LED | FULL_SPEED_LED);
  P2SEL &= ~(LOW_SPEED_LED | MID_SPEED_LED | FULL_SPEED_LED);
  P2SEL2 &= ~(LOW_SPEED_LED | MID_SPEED_LED | FULL_SPEED_LED);

  // P1.6 = TA0.1 -> TA0CCR1
  P1DIR |= SPEED_OUTPUT;   // P1.0 configured as output
  P1SEL |= SPEED_OUTPUT;   // Primary peripheral module function for P1.6
  P1SEL2 &= ~SPEED_OUTPUT; // Primary peripheral module function for P1.6

  TA0CTL &= ~MC_0;      /* Timer A mode control: 0 - Stop */
  TA0CTL |= TASSEL_1;   /* Timer A clock source select: 1 - ACLK  */
  TA0CTL |= ID_0;       /* Timer A input divider: 0 - /1 */
  TA0CCTL1 |= OUTMOD_7; /* PWM output mode: 7 - PWM reset/set */
  TA0CCR0 = FULL_SPEED_VALUE;
  TA0CCR1 = NO_SPEED_VALUE;
  TA0CTL |= MC_1; /* Timer A mode control: 1 - Up to CCR0 */

  __bis_SR_register(LPM3_bits + GIE);
}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void) {
  if (P1IFG & SPEED_PUSH_BUTTON) {
    TA0CTL &= ~MC_0;

    switch (TA0CCR1) {
    case NO_SPEED_VALUE:
      TA0CCR1 = LOW_SPEED_VALUE;
      P2OUT |= LOW_SPEED_LED;
      break;

    case LOW_SPEED_VALUE:
      P2OUT &= ~LOW_SPEED_LED;
      TA0CCR1 = MID_SPEED_VALUE;
      P2OUT |= MID_SPEED_LED;
      break;

    case MID_SPEED_VALUE:
      P2OUT &= ~MID_SPEED_LED;
      TA0CCR1 = FULL_SPEED_VALUE;
      P2OUT |= FULL_SPEED_LED;
      break;

    case FULL_SPEED_VALUE:
      P2OUT &= ~FULL_SPEED_LED;
      TA0CCR1 = NO_SPEED_VALUE;
      break;
    }

    TA0CTL |= MC_1;

    P1IFG &= ~SPEED_PUSH_BUTTON;
  }
}

#pragma vector = PORT2_VECTOR
__interrupt void Port2_ISR(void) {
  if (P2IFG & ENABLE_PUSH_BUTTON) {
    static int currentStatus = 0;

    if (TA0CTL & MC_1) {
      TA0CTL &= ~MC_1;
    } else {
      TA0CTL |= MC_1;
    }

    if (P2OUT & LOW_SPEED_LED) {
      currentStatus = LOW_SPEED_LED;
      P2OUT &= ~LOW_SPEED_LED;
    } else if (P2OUT & MID_SPEED_LED) {
      currentStatus = MID_SPEED_LED;
      P2OUT &= ~MID_SPEED_LED;
    } else if (P2OUT & FULL_SPEED_LED) {
      currentStatus = FULL_SPEED_LED;
      P2OUT &= ~FULL_SPEED_LED;
    } else {
      P2OUT |= currentStatus;
    }

    P2IFG &= ~ENABLE_PUSH_BUTTON;
  }
}
