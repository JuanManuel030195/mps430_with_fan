#include <intrinsics.h>
#include <stdbool.h>

#include <msp430g2553.h>

#include "./State.h"

#define NO_SPEED_VALUE 0
#define LOW_SPEED_VALUE 4
#define MID_SPEED_VALUE 7
#define FULL_SPEED_VALUE 10

#define ENABLE_PUSH_BUTTON BIT7
#define SPEED_PUSH_BUTTON BIT3
#define SPEED_OUTPUT BIT6

#define LOW_SPEED_LED BIT3
#define MID_SPEED_LED BIT1
#define FULL_SPEED_LED BIT5 /* ACLK/ID_0/(TA0CCR0 + 1) = Period in Hz */

#define TOTAL_STATES 4

volatile bool changeSpeedButtonPressed = false;
volatile bool enableSpeedButtonPressed = false;

volatile unsigned int currentState;

volatile struct State appState;

const struct State appStates[TOTAL_STATES] = {
    {NO_SPEED_VALUE, 0, false},
    {LOW_SPEED_VALUE, LOW_SPEED_LED, true},
    {MID_SPEED_VALUE, MID_SPEED_LED, true},
    {FULL_SPEED_VALUE, FULL_SPEED_LED, true}};

const unsigned char NON_USED_P1_PINS = BIT0 | BIT1 | BIT2 | BIT4 | BIT5 | BIT7;
const unsigned char NON_USED_P2_PINS = BIT0 | BIT2 | BIT4 | BIT6;

int main(void) {
  WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

  currentState = 0;
  appState = *appStates;

  // Setup ACLK to 12KHz
  BCSCTL3 |= LFXT1S_2; /* Mode 2 for LFXT1 : VLOCLK 12KHz*/
  BCSCTL1 |= DIVA_0;   /* ACLK Divider 0: /1 */

  // Configure P1 initially as unused port to reduce power consumption
  P1DIR &= ~NON_USED_P1_PINS;  // Configured as output
  P1SEL &= ~NON_USED_P1_PINS;  // I/O function
  P1SEL2 &= ~NON_USED_P1_PINS; // I/O function
  P1REN |= NON_USED_P1_PINS;   // Pullup or pulldown resistor enabled
  P1OUT &= ~NON_USED_P1_PINS;  // Pulldown resistor selected

  // Configure P2 initially as unused port to reduce power consumption
  P2DIR &= ~NON_USED_P2_PINS;  // Configured as output
  P2SEL &= ~NON_USED_P2_PINS;  // I/O function
  P2SEL2 &= ~NON_USED_P2_PINS; // I/O function
  P2REN |= NON_USED_P2_PINS;   // Pullup or pulldown resistor enabled
  P2OUT &= ~NON_USED_P2_PINS;  // Pulldown resistor selected

  // P1.0 = ACLK
  // P1DIR |= BIT0;   // P1.0 configured as output
  // P1SEL |= BIT0;   // Primary peripheral module function for P1.0
  // P1SEL2 &= ~BIT0; // Primary peripheral module function for P1.0

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

  TA0CTL &= ~MC_0;            /* Timer A mode control: 0 - Stop */
  TA0CTL |= TASSEL_1;         /* Timer A clock source select: 1 - ACLK  */
  TA0CTL |= ID_0;             /* Timer A input divider: 0 - /1 */
  TA0CCTL1 |= OUTMOD_7;       /* PWM output mode: 7 - PWM reset/set */
  TA0CCR0 = FULL_SPEED_VALUE; /* 1KHz PWM Period */
  TA0CCR1 = appState.speed;   /* 0% Duty cicle */
  // TA0CTL |= MC_1;             /* Timer A mode control: 1 - Up to CCR0 */

  TA1CTL &= ~MC_0;      /* Timer A mode control: 0 - Stop */
  TA1CTL |= TASSEL_1;   /* Timer A clock source select: 1 - ACLK  */
  TA1CTL |= ID_3;       /* Timer A input divider: 3 - /8 */
  TA1CCTL0 |= OUTMOD_3; /* PWM output mode: 3 - PWM set/reset */
  TA1CCR0 = 750 - 1;
  TA1CCR1 = 0;
  TA1CTL &= ~TAIFG; /* Clear Timer A counter interrupt flag */
  TA1CTL |= TAIE;   /* Timer A counter interrupt enable */
  // TA1CTL |= MC_1;   /* Timer A mode control: 1 - Up to CCR0 */

  __enable_interrupt();

  __bis_SR_register(LPM4_bits);
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer1_A1_ISR(void) {

  if (TA1IV & TA1IV_TACCR1) {
    TA1CTL &= ~MC_1; // Stop debounce timer
    TA1CTL |= MC_0;  // Stop debounce timer

    TA0CTL &= ~MC_1; // Stop TA0 so we can make changes
    TA0CTL |= MC_0;  // Stop TA0 so we can make changes

    if (changeSpeedButtonPressed) {
      changeSpeedButtonPressed = false;

      if (!appState.enabled && currentState != 0) {
        appState.enabled = true;
      } else {
        if (currentState < TOTAL_STATES - 1) {
          currentState++;
        } else {
          currentState = 0;
        }

        P2OUT &= ~appState.indicator;

        appState = *(appStates + currentState);
      }

      P2OUT |= appState.indicator;
      TA0CCR1 = appState.speed;
    }

    if (enableSpeedButtonPressed) {
      enableSpeedButtonPressed = false;

      if (appState.enabled) {
        P2OUT &= ~appState.indicator;
        TA0CCR1 = NO_SPEED_VALUE;
      } else {
        P2OUT |= appState.indicator;
        TA0CCR1 = appState.speed;
      }

      appState.enabled ^= true;
    }

    TA0CTL |= MC_1; // Start TA0 so changes can take effect

    if (appState.enabled) {
      __bic_SR_register(OSCOFF); // Go LPM3 so we can have PWM
    } else {
      __bis_SR_register(OSCOFF); // No Speed set, go LPM4
    }

    TA1IV &= ~TA1IV_TACCR1;
  }
}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void) {
  if (P1IFG & SPEED_PUSH_BUTTON) {
    enableSpeedButtonPressed = false;
    changeSpeedButtonPressed = true;

    TA1CTL &= ~MC_1; // Stop debounce timer
    TA1CTL |= MC_0;  // Stop debounce timer

    TA1R = 0; // Restart debounce timer count

    TA1CTL |= MC_1; // Start debounce timer Up to CCR0

    // Go LPM3 on exit so debounce timer can start count
    __bic_SR_register_on_exit(OSCOFF);

    P1IFG &= ~SPEED_PUSH_BUTTON;
  }
}

#pragma vector = PORT2_VECTOR
__interrupt void Port2_ISR(void) {
  if (P2IFG & ENABLE_PUSH_BUTTON) {
    changeSpeedButtonPressed = false;
    enableSpeedButtonPressed = true;

    TA1CTL &= ~MC_1; // Stop debounce timer
    TA1CTL |= MC_0;  // Stop debounce timer

    TA1R = 0; // Restart debounce timer count

    TA1CTL |= MC_1; // Start debounce timer Up to CCR0

    // Go LPM3 on exit so debounce timer can start count
    __bic_SR_register_on_exit(OSCOFF);

    P2IFG &= ~ENABLE_PUSH_BUTTON;
  }
}
