
#ifndef MOTORLIB_H
#define	MOTORLIB_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "mcc_generated_files/mcc.h"

typedef enum{
    HALT, 
    ACC, 
    CONST, 
    DEC
} Motor_states;

void clr_string(char* str, uint8_t strlen);

void dir_toggle();

void dir_fwd(void);

void dir_rev(void);

void motor_move(uint32_t pulses);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

