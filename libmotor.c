#include "libmotor.h"

extern volatile uint16_t ramp_up_pulses;
extern volatile uint24_t nco_increment_fmin;
extern volatile Motor_states motor_state;

void clr_string(char* str, uint8_t strlen){
    for(uint8_t i = 0; i < strlen; i++){
        str[i] = '\0';
    }
}

void dir_toggle(){
    IO_DIR_Toggle();
    IO_DIR_NEG_Toggle();
    if(IO_DIR_GetValue()){
        putch('r');
    }else{
        putch('f');
    }
}

void dir_fwd(void){
    IO_DIR_SetLow();
    IO_DIR_NEG_SetHigh();
    putch('f');
}

void dir_rev(void){
    IO_DIR_SetHigh();
    IO_DIR_NEG_SetLow();
    putch('r');
}

void motor_move(uint16_t pulses){
    uint16_t total_pulses = pulses;
    uint16_t realtime_pulse_count;
    uint16_t tmrLoadVal;
    uint16_t ramp_length;
    
    NCO1_Write(nco_increment_fmin);
    motor_state = ACC;
    
    tmrLoadVal = UINT16_MAX - pulses + 1;
    TMR1_WriteTimer(tmrLoadVal);
    TMR1_StartTimer();
    TMR0_StartTimer();
    NCO1_Start();
    
    while(NCO1_Enabled()){
        realtime_pulse_count = TMR1_ReadTimer() - tmrLoadVal;
        if((motor_state == ACC) && ((total_pulses / 2) <= realtime_pulse_count)){
            motor_state = DEC;
            TMR0_Reload();
            TMR0_StartTimer();
            putch(0x00);
        }else if(motor_state == CONST){
            ramp_length = ramp_up_pulses - tmrLoadVal;
            if((total_pulses - realtime_pulse_count) <= ramp_length){
                motor_state = DEC;
                TMR0_Reload();
                TMR0_StartTimer();
                putch(0x00);
            }
        }
    };
    motor_state = HALT;
}