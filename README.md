# MSP430 Fan Control System

This project implements a fan control system using the MSP430G2553 microcontroller. The system allows for controlling the fan speed through push buttons and provides visual feedback using LEDs.

## Features

- **Fan Speed Control**: Adjust the fan speed using a push button.
- **Enable/Disable Fan**: Enable or disable the fan using another push button.
- **LED Indicators**: Visual feedback for different fan speeds using LEDs.
- **Low Power Consumption**: Unused ports are configured to reduce power consumption.

## Hardware Setup

- **MSP430G2553 Microcontroller**
- **Push Buttons**:
  - Speed Push Button: Connected to P1.3
  - Enable Push Button: Connected to P2.7
- **LEDs**:
  - Low Speed LED: Connected to P2.3
  - Mid Speed LED: Connected to P2.1
  - Full Speed LED: Connected to P2.5
- **Fan Control**:
  - PWM Output: Connected to P1.6

## Software Setup

### Constants

- `NO_SPEED_VALUE`: 0
- `LOW_SPEED_VALUE`: 4
- `MID_SPEED_VALUE`: 7
- `FULL_SPEED_VALUE`: 10

### Pin Definitions

- `ENABLE_PUSH_BUTTON`: BIT7
- `SPEED_PUSH_BUTTON`: BIT3
- `SPEED_OUTPUT`: BIT6
- `LOW_SPEED_LED`: BIT3
- `MID_SPEED_LED`: BIT1
- `FULL_SPEED_LED`: BIT5

### State Structure

The state structure `State` is defined in `State.h` and includes:
- `speed`: Fan speed value
- `indicator`: LED indicator
- `enabled`: Fan enabled status

### Main Function

The `main` function initializes the microcontroller, configures the ports, sets up timers, and enters low power mode.

### Interrupt Service Routines

- **Timer1_A1_ISR**: Handles debounce timer and updates the fan state based on button presses.
- **Port1_ISR**: Handles the speed push button interrupt.
- **Port2_ISR**: Handles the enable push button interrupt.

## Usage

1. **Compile and upload the code** to the MSP430G2553 microcontroller.
2. **Connect the hardware** as described in the hardware setup section.
3. **Press the Speed Push Button** to cycle through different fan speeds.
4. **Press the Enable Push Button** to enable or disable the fan.

## License

This project is licensed under the MIT License.