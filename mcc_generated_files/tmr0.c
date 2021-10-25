#include <xc.h>
#include "tmr0.h"
#include "nco1.h"
#include "pin_manager.h"
#include "tmr1.h"



/**
  Section: Global Variables Definitions
*/

volatile uint16_t timer0ReloadVal16bit;

extern volatile uint16_t ramp_up_pulses;
extern volatile uint24_t nco_increment_fmax;
extern volatile uint24_t nco_increment_fmin;
extern volatile uint24_t nco_ramp_increment;
extern volatile Motor_states motor_state;

/**
  Section: TMR0 APIs
*/

void (*TMR0_InterruptHandler)(void);

void TMR0_Initialize(void)
{
    // Set TMR0 to the options selected in the User Interface

    // T0CS FOSC/4; T0CKPS 1:4; T0ASYNC synchronised; 
    T0CON1 = 0x42;

    // TMR0H 99; 
    TMR0H = 0x63;

    // TMR0L 192; 
    TMR0L = 0xC0;

    // Load TMR0 value to the 16-bit reload variable
    timer0ReloadVal16bit = (TMR0H << 8) | TMR0L;

    // Clear Interrupt flag before enabling the interrupt
    PIR3bits.TMR0IF = 0;

    // Enabling TMR0 interrupt.
    PIE3bits.TMR0IE = 1;

    // Set Default Interrupt Handler
    TMR0_SetInterruptHandler(TMR0_DefaultInterruptHandler);

    // T0OUTPS 1:1; T0EN disabled; T016BIT 16-bit; 
    T0CON0 = 0x10;
}

void TMR0_StartTimer(void)
{
    // Start the Timer by writing to TMR0ON bit
    T0CON0bits.T0EN = 1;
}

void TMR0_StopTimer(void)
{
    // Stop the Timer by writing to TMR0ON bit
    T0CON0bits.T0EN = 0;
    IO_RB0_SetLow();
}

uint16_t TMR0_ReadTimer(void)
{
    uint16_t readVal;
    uint8_t readValLow;
    uint8_t readValHigh;

    readValLow  = TMR0L;
    readValHigh = TMR0H;
    readVal  = ((uint16_t)readValHigh << 8) + readValLow;

    return readVal;
}

void TMR0_WriteTimer(uint16_t timerVal)
{
    // Write to the Timer0 register
    TMR0H = timerVal >> 8;
    TMR0L = (uint8_t) timerVal;
}

void TMR0_Reload(void)
{
    // Write to the Timer0 register
    TMR0H = timer0ReloadVal16bit >> 8;
    TMR0L = (uint8_t) timer0ReloadVal16bit;
}

void TMR0_ISR(void)
{
    // clear the TMR0 interrupt flag
    PIR3bits.TMR0IF = 0;
    // Write to the Timer0 register
    TMR0H = timer0ReloadVal16bit >> 8;
    TMR0L = (uint8_t) timer0ReloadVal16bit;

    // ticker function call;
    // ticker is 1 -> Callback function gets called every time this ISR executes
    TMR0_CallBack();

    // add your TMR0 interrupt custom code
}

void TMR0_CallBack(void)
{
    // Add your custom callback code here

    if(TMR0_InterruptHandler)
    {
        TMR0_InterruptHandler();
    }
}

void TMR0_SetInterruptHandler(void (* InterruptHandler)(void)){
    TMR0_InterruptHandler = InterruptHandler;
}

void TMR0_DefaultInterruptHandler(void){
    // add your TMR0 interrupt custom code
    // or set custom function using TMR0_SetInterruptHandler()
            
    if(motor_state == ACC){
        if(NCO1_Increase(nco_ramp_increment, nco_increment_fmax)){
            IO_RB0_SetHigh();
        }else{
            TMR0_StopTimer();
            // mark the amount of pulses
            ramp_up_pulses = TMR1_ReadTimer();
            motor_state = CONST;
        }
    }else if(motor_state == DEC){
        // decrease until stop
        if(NCO1_Decrease(nco_ramp_increment, nco_increment_fmin)){
            IO_RB0_SetHigh();
        }else{
            TMR0_StopTimer();
        }
    }
}

/**
  End of File
*/