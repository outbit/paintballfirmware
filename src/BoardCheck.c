/*
   ----------------------
   BoardCheck.c
   ----------------------
   Created: 10/24/04
   Updated: 07/03/15
   Version: 1.4
   ----------------------
   Compiler: PICC 9.40 C Compiler From HI-TECH
   Assembler: PICC 9.40 Assembler From HI-TECH
   Linker: PICC Linker From HI-TECH
   IDE: MPLAB IDE v6.62
   ----------------------
   Author: David Lee Whiteside
   Project: CodeX DM4/5/C Chip(TM)
   Target MCU: PIC16F648a From Microchip
   ----------------------
   Copyright (c) 2004-2015 David Whiteside

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 */
#include <BoardCheck.h>
#include <CustomDM4.h> // FOR BLINK TESTING


BYTE G_Board_Type;
BYTE G_Board_Battery;
bank1 LBI_FUNCTION_GET _DIP1_ISSET_PTR = 0;
bank1 LBI_FUNCTION_GET _EYEBUTTON_ISDOWN_PTR = 0;
bank1 LBI_FUNCTION_SET _EYEPOWER_ON_PTR = 0;
bank1 LBI_FUNCTION_SET _EYEPOWER_OFF_PTR = 0;


BOOL _NONLBI_DIP1_ISSET(void)
{
    return NONLBI_DIP1_ISSET();
}
BOOL _LBI_DIP1_ISSET(void)
{
    return LBI_DIP1_ISSET();
}

BOOL _NONLBI_EYEBUTTON_ISDOWN(void)
{
    return NONLBI_EYEBUTTON_ISDOWN();
}
BOOL _LBI_EYEBUTTON_ISDOWN(void)
{
    return LBI_EYEBUTTON_ISDOWN();
}

void _NONLBI_EYEPOWER_ON(void)
{
    NONLBI_EYEPOWER_ON();
}
void _LBI_EYEPOWER_ON(void)
{
    LBI_EYEPOWER_ON();
}
void _NONLBI_EYEPOWER_OFF(void)
{
    NONLBI_EYEPOWER_OFF();
}
void _LBI_EYEPOWER_OFF(void)
{
    LBI_EYEPOWER_OFF();
}

void _CHOOSEBOARD(void)
{
    TRISA = 0x00; // PORTA Input/Output
    NOP();
    PORTA = 0x02; // PORTA
    NOP();
    TRISA = 0x02; // PORTA Input/Output
    NOP();

    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();

    // Check Board Type
    if (RA1 == 0) {
        G_Board_Type = NONLBIBOARD; // NON-LBI

        TRISA = 0x00; // PORTA Input/Output Setting
        TRISB = 0x03; // PORTB Input/Output Setting
        NOP();
        PORTA = 0x16; // PORTA Default
        PORTB = 0x4F; // PORTB Default
        NOP();
        // Using Puulup Resistor For DIP1 and DIP4
        TRISA3 = 0;
        RA3 = 1;
        TRISA7 = 1;
        TRISA6 = 1;
        TRISA2 = 1; // Eye SW
        TRISB3 = 1;
        TRISB2 = 1;


        _DIP1_ISSET_PTR = _NONLBI_DIP1_ISSET;
        _EYEBUTTON_ISDOWN_PTR = _NONLBI_EYEBUTTON_ISDOWN;
        _EYEPOWER_ON_PTR = _NONLBI_EYEPOWER_ON;
        _EYEPOWER_OFF_PTR = _NONLBI_EYEPOWER_OFF;
    } else {
        G_Board_Type = LBIBOARD; // LBI

        TRISA = 0x26; // PORTA Input/Output Setting
        TRISB = 0x03; // PORTB Input/Output Setting
        NOP();
        PORTA = 0xAC; // PORTA Default
        PORTB = 0x4F; // PORTB Default
        NOP();
        // Using Puulup Resistor For DIP1 and DIP4
        TRISA3 = 0;
        RA3 = 1;
        TRISA4 = 1;
        TRISA6 = 1;
        TRISA5 = 1; // Eye SW
        TRISB3 = 1;
        TRISB2 = 1;

        _DIP1_ISSET_PTR = _LBI_DIP1_ISSET;
        _EYEBUTTON_ISDOWN_PTR = _LBI_EYEBUTTON_ISDOWN;
        _EYEPOWER_ON_PTR = _LBI_EYEPOWER_ON;
        _EYEPOWER_OFF_PTR = _LBI_EYEPOWER_OFF;
    }

    // Full Battery Voltage
    G_Board_Battery = FULLBATTERY;
//    RBPU = 0;                // PORTB Internal Pullup (Hi-Tech C)
    OPTION_REG &= ~(1<<7);     // PORTB Internal Pullup (mpLab)
}

void _COMPARE(BYTE vr)
{
    CMCON = 0x0D;      // 101 Comparator
    VRCON = 0xC0 | vr; // Voltage Reference For Low Battery Indicator 11101100
    NOP();
    wait(100);

    if (C2OUT) {
        G_Board_Battery--;
    }

    CMCON = 0x07;   // No Comparator
    VRCON = 0x00;   // Voltage Reference For Low Battery Indicator 00000000
    NOP();
}

void _CHECKBATTERY(void)
{
    BYTE x;

// Delay
    wait(HALF_SECOND);

    if (G_Board_Type == LBIBOARD) {
        _COMPARE(0x0F);
        _COMPARE(0x0D);
        _COMPARE(0x0C);
    }

// Look Like Eye Delay
    for (x = 0; x < G_Board_Battery; x++)
        BLINK(RED);
}

void _POWERSAVE(void)
{
    if (G_Board_Type == LBIBOARD) {
        PORTA = 0x00;
        PORTB = 0x00;
    } else {
        PORTA = 0x24;
        PORTB = 0x00;
    }
}
