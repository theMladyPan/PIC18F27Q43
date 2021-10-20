#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <stdio.h>


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

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    printf("Rdy!");
    
    char *text;
    char rx_buffer[8];
    uint32_t pulseAmount = 0;
    
    
    while(1){
        while(0 == uart1RxCount){
            __delay_us(10);
        }
        
        int ib = 0;
        int loopctr = 0;
        char rcvd;
        clr_string(rx_buffer, sizeof(rx_buffer));
        
        // wait for STX char
        do{
            rcvd = UART1_Read();
        }while(rcvd != 0x01);
        
        rx_buffer[ib++] = rcvd;
        
        while(rcvd != 0x04){
            //receive message into buffer until EOT is transmitted
            rcvd = UART1_Read();
            rx_buffer[ib++] = rcvd;
            if(rcvd == 0x04) break;
        }
        
        //send ACK
        putch(0x06);
        
        //handle received data (big-endian)
        for(uint8_t c = 0; c < ib; c){
            if(rx_buffer[c] == 'l'){
                NCO1_Stop();
                pulseAmount = 0;
                for(uint8_t i = 0; i < 4; i++){
                    uint8_t shift = ((3-i)*8);
                    uint8_t buf = (uint8_t)rx_buffer[++c];
                    uint32_t increment =  buf << shift;
                    pulseAmount += increment;
                }
                printf("%d", pulseAmount);
            }
            if(rx_buffer[c] == 's'){
                while(pulseAmount > UINT16_MAX){
                    TMR1_WriteTimer(0);
                    TMR1_StartTimer();
                    NCO1_Start();
                    pulseAmount -= 0x10000;
                    while(NCO1_Enabled());
                }
                if(pulseAmount < UINT16_MAX){
                    uint16_t tmrLoadVal = UINT16_MAX - pulseAmount + 1;
                    TMR1_WriteTimer(tmrLoadVal);
                    TMR1_StartTimer();
                    NCO1_Start();
                }
                while(NCO1_Enabled());
            }
            if(rx_buffer[c] == 't'){
                dir_toggle();
            }
            if(rx_buffer[c] == 'r'){
                dir_rev();
            }
            if(rx_buffer[c] == 'f'){
                dir_fwd();
            }
            c++;
        }
        putch(0x03);
    }
}
/*
    while (1)
    {
        if(UART1_is_rx_ready()){
            tmp = UART1_Read();
            switch(tmp)
            {
                case 'p':{
                    pulseAmount = (uint16_t)UART1_Read() << 8;
                    pulseAmount += UART1_Read();
                    write_string(putch, itoa(pulseAmount));
                    break;
                }
                case 'l':
                {
                    TMR1_StopTimer();
                    NCO1CONbits.EN = 0;
                    uint16_t tmrLoadVal = UINT16_MAX - pulseAmount + 1;
                    TMR1_WriteTimer(tmrLoadVal);
                    break;
                }
                case 's':
                {
                    // TMR1_Reload();
                    TMR1_StartTimer();
                    NCO1CONbits.EN = 1;
                    break;
                }
                case 'r':
                {
                    NCO1CONbits.EN = 0;
                    TMR1_StopTimer();
                    UART1_Write(TMR1H);
                    UART1_Write(TMR1L);
                    break;
                }
                default:
                {
                    UART1_Write(tmp);
                    break;
                }
            }
        }        
    }*/