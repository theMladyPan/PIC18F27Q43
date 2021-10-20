#include "mcc_generated_files/mcc.h"



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

    uint8_t tmp, tmp2;
    uint8_t hello[] = "hello";
    

    uint16_t pulseAmount = 0;
    
    for(int i=0;i<6;i++){
        while(!UART1_is_tx_ready());
        UART1_Write(hello[i]);
    }

    while (1)
    {
        if(UART1_is_rx_ready()){
            tmp = UART1_Read();
            switch(tmp)
            {
                case 't':
                {
                    // Enable the NCO module
                    NCO1CONbits.EN = ~NCO1CONbits.EN;
                    break;
                }
                case 'p':{
                    pulseAmount = UART1_Read() << 8;
                    pulseAmount += UART1_Read();
                    UART1_Write('O');
                    UART1_Write('K');
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
        }/*else{
            UART1_Write(TMR1H);
            __delay_ms(100);
        }*/
        
    }
}
/**
 End of File
*/