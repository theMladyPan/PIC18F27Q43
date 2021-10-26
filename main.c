#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <stdio.h>
#include "libmotor.h"

//user defined
#define NCO_DEFAULT_RAMP_MS 2000 //ms
#define NCO_MIN_FREQ 100 //Hz
#define NCO_MAX_FREQ 21362 //Hz
#define TMR0_INT_PERIOD_MS 1 //ms
#define NCO_CLOCK_SOURCE_FREQ 500000//( _XTAL_FREQ )


volatile float nco_ramp_increment;
volatile float ramp_increase_count = ( NCO_DEFAULT_RAMP_MS / TMR0_INT_PERIOD_MS );
volatile uint24_t nco_increment_fmin = (( NCO_MIN_FREQ * 0x100000 ) / NCO_CLOCK_SOURCE_FREQ ) * 2;
volatile uint24_t nco_increment_fmax = (( (uint64_t)NCO_MAX_FREQ * 0x100000 ) / NCO_CLOCK_SOURCE_FREQ ) * 2;
volatile uint24_t nco_increment_delta;
volatile uint16_t ramp_up_pulses = UINT16_MAX;
volatile Motor_states motor_state = HALT;


void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    
    
// precomputed
    nco_increment_delta = ( nco_increment_fmax - nco_increment_fmin );
    nco_ramp_increment = ( nco_increment_delta / ramp_increase_count );
    if (nco_ramp_increment <= 0) nco_ramp_increment = 1;
    

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    

    // Disable the Global Interrupts
    // INTERRUPT_GlobalInterruptDisable();
    // Khoko papa
    
    putch(0x01);
    putch(0x04);
    
    char rx_buffer[8];
    uint32_t pulseAmount = 0;
    
    
    while(1){
        while(0 == uart1RxCount){
            __delay_us(10);
        }
        
        int ib = 0;
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
        putch(0x02);
        
        //handle received data (big-endian)
        for(uint8_t c = 0; c < ib; c++){            
            if(rx_buffer[c] == 'l'){
                pulseAmount = rx_buffer[++c] << 8 | (uint8_t)rx_buffer[++c];
                printf("%X", (uint16_t)pulseAmount);
            }
            
            if(rx_buffer[c] == 's'){
                putch(0x06);
                motor_move(pulseAmount);
            }
            
            if(rx_buffer[c] == 'a'){
                uint16_t ramp_duration = rx_buffer[++c] << 8 | (uint8_t)rx_buffer[++c];
                printf("%X", ramp_duration);
                ramp_increase_count = (ramp_duration / TMR0_INT_PERIOD_MS);
                nco_ramp_increment = ( nco_increment_delta / ramp_increase_count ); 
                if(nco_ramp_increment<=0)nco_ramp_increment = 1;
            }
           
            if(rx_buffer[c] == 't'){
                putch(0x06);
                dir_toggle();
            }
            
            if(rx_buffer[c] == 'r'){
                putch(0x06);
                dir_rev();
            }
            
            if(rx_buffer[c] == 'f'){
                putch(0x06);
                dir_fwd();
            }
        }
        putch(0x04);
    }
}