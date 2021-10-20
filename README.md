# UART/Stepper driver

## Usage
receive command over uart. Default is 115200 baud 1 stop no parity.

command always begins with <SOH> = 0x01 and ends with <EOT> = 0x04. Syntax is as follows:

`<SOH>l####<ETX>` - where #### are four bytes (uint32, MSB first) of desired location
`<SOH>s<ETX>` - execute movement
`<SOH>f<ETX>` - forward direction
`<SOH>r<ETX>` - reverse direction
`<SOH>t<ETX>` - toggle direction (fwd/rev)

## GPIO Pinout:
NCO - oscillator pulse output on pin12 = RC1
DIR - direction output on pin13 = RC2
DIR_NEG - negated direction output on pin14 = RC3
