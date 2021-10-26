/**
   NCO1 Generated Driver File
 
   @Company
     Microchip Technology Inc.
 
   @File Name
     nco1.c
 
   @Summary
     This is the generated driver implementation file for the NCO1 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs
 
   @Description
     This source file provides implementations for driver APIs for NCO1.
     Generation Information :
         Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
         Device            :  PIC18F27Q43
         Driver Version    :  2.11
     The generated drivers are tested against the following:
         Compiler          :  XC8 2.31 and above or later
         MPLAB             :  MPLAB X 5.45
 */ 

 /*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/
 
 /**
   Section: Included Files
 */

#include <xc.h>
#include "nco1.h"

extern volatile Motor_states motor_state;

/**
  Section: NCO Module APIs
*/

void NCO1_Initialize (void)
{
    // Set the NCO to the options selected in the GUI
    // EN disabled; POL active_hi; PFM FDC_mode; 
    NCO1CON = 0x00;
    // CKS MFINTOSC_500KHz; PWS 1_clk; 
    NCO1CLK = 0x03;
    // 
    NCO1ACCU = 0x00;
    // 
    NCO1ACCH = 0x00;
    // 
    NCO1ACCL = 0x00;
    // 
    NCO1INCU = 0x00;
    // 
    NCO1INCH = 0x00; //1khz
    //NCO1INCH = 0x09; //72khz
    // 
    NCO1INCL = 0x21; //1kHz
    //NCO1INCL = 0x37; //72kHz

   
}

void NCO1_Write(uint24_t NcoVal)
{
    // Write to the Timer0 register
    NCO1INCU = (uint8_t) (NcoVal >> 16);
    NCO1INCH = (uint8_t) (NcoVal >> 8);
    NCO1INCL = (uint8_t) NcoVal;
}

bool NCO1_Increase(uint24_t ncoIncVal, uint24_t maxInc)
{
    uint24_t ncoVal;
    ncoVal = NCO1_Read();
    if(ncoVal < maxInc){
        if((ncoVal + ncoIncVal) < maxInc){
            NCO1_Write(ncoVal + ncoIncVal);
            return true;
        }else{
            NCO1_Write(maxInc);
            return false;
        }
    }else{
        return false;
    }      
}
bool NCO1_Decrease(uint24_t ncoIncVal, uint24_t minInc)
{
    uint24_t ncoVal;
    ncoVal = NCO1_Read();
    if(ncoVal > minInc){
        if((ncoVal - ncoIncVal) > minInc){
            NCO1_Write(ncoVal - ncoIncVal);
        return true;
        }else{
            NCO1_Write(minInc);
            return true;
        }
    }else{
        return false;
    }      
}

uint24_t NCO1_Read(){
    uint24_t readVal;
    uint8_t readValLow;
    uint8_t readValHigh;
    uint8_t readValTop;

    readValLow  = NCO1INCL;
    readValHigh = NCO1INCH;
    readValTop  = NCO1INCU;
    readVal  = ((uint24_t)readValTop << 16) + ((uint16_t)readValHigh << 8) + (uint8_t)readValLow;

    return readVal;
}

bool NCO1_GetOutputStatus(void)
{
    // Return output status on accumulator over flow
    return (NCO1CONbits.OUT);
}

void NCO1_Stop(void){
    NCO1CONbits.EN = 0;
    motor_state = HALT;
}

void NCO1_Start(void){
    NCO1CONbits.EN = 1;
}

bool NCO1_Enabled(void){
    return NCO1CONbits.EN;
}