/*
----------------------
DM4Macros.h
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
#ifndef _DM4MACROS_H
#define _DM4MACROS_H

#define RAMPING


#include <pic.h>


// Standard Defines
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL  0
#endif

// Standard Typedefs
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef BYTE           BOOL;


// PIC Macros
#ifndef NOP   // Null Operation
#define NOP()    asm("nop")
#endif
#ifndef EI    // Global Interrupt Enable
#define EI()     (GIE = TRUE)
#endif
#ifndef DI    // Global Interrupt Disable
#define DI()     (GIE = FALSE)
#endif
#ifndef SLEEP // Sleep Mode
#define	SLEEP()   asm("sleep")
#endif
#ifndef CLRWDT // Control Watch Dog Timer
#define	CLRWDT()  asm("clrwdt")
#endif
#ifndef KICKDOG // Control Watch Dog Timer
#define	KICKDOG()  CLRWDT()
#endif


// DM4 Inputs
#define IN_TRIGGER     RB1
#define IN_EYEINPUT    RB0
#define IN_POWERBUTTON RB6
#define IN_EYEBUTTON   RA2
#define IN_DIP1        RA7
#define IN_DIP2        RB3
#define IN_DIP3        RB2
#define IN_DIP4        RA6

// DM4 Outputs
#define OUT_LEDBLUE    RB4
#define OUT_LEDRED     RB5
#define OUT_LEDGREEN   RA0
#define OUT_SOLENOID   RB7
#define OUT_EYEPOWER   RA1


// DM4 Input Macros
#define TRIGGER_ISDOWN()     (IN_TRIGGER == 0)				// Active Low
#define EYEINPUT_ISBROKE()   (IN_EYEINPUT == 1)				// Active High
#define POWERBUTTON_ISDOWN() (IN_POWERBUTTON == 0)			// Active Low
#define EYEBUTTON_ISDOWN()   _EYEBUTTON_ISDOWN_PTR()		// Active Low
#define DIP1_ISSET()         _DIP1_ISSET_PTR()				// Active Low
#define DIP2_ISSET()         (IN_DIP2 == 0)					// Active Low
#define DIP3_ISSET()         (IN_DIP3 == 0)					// Active Low
#define DIP4_ISSET()         (IN_DIP4 == 0)					// Active Low

// DM4 Output Macros
#define LEDBLUE_ON()         (OUT_LEDBLUE = 1)				// Active High
#define LEDBLUE_OFF()        (OUT_LEDBLUE = 0)
#define LEDRED_ON()          (OUT_LEDRED = 1)				// Active High
#define LEDRED_OFF()         (OUT_LEDRED = 0)
#define LEDGREEN_ON()        (OUT_LEDGREEN = 1)				// Active High
#define LEDGREEN_OFF()       (OUT_LEDGREEN = 0)
#define SOLENOID_ON()        (OUT_SOLENOID = 1)				// Active High
#define SOLENOID_OFF()       (OUT_SOLENOID = 0)
#define EYEPOWER_ON()        _EYEPOWER_ON_PTR()				// Active High
#define EYEPOWER_OFF()       _EYEPOWER_OFF_PTR()


// NON-LBI Board
#define NONLBI_DIP1_ISSET()       (IN_DIP1 == 0)                // NON-LBI Active Low
#define NONLBI_EYEPOWER_ON()      TRISB0 = 1;OUT_EYEPOWER = 1;NOP();  // NON-LBI Active High
#define NONLBI_EYEPOWER_OFF()     TRISB0 = 0;OUT_EYEPOWER = 0;NOP();  // NON-LBI Active High
#define NONLBI_EYEBUTTON_ISDOWN() (IN_EYEBUTTON == 0)           // NON-LBI Active Low

// LBI Board
#define LBI_IN_DIP1              RA4                                    // LBI
#define LBI_DIP1_ISSET()         (LBI_IN_DIP1 == 0)						// LBI Active Low
#define LBI_OUT_EYEPOWER         RA7                                    // LBI
#define LBI_EYEPOWER_ON()        TRISB0 = 1;LBI_OUT_EYEPOWER = 1;NOP(); // LBI Active High
#define LBI_EYEPOWER_OFF()       TRISB0 = 0;LBI_OUT_EYEPOWER = 0;NOP(); // LBI Active High
#define LBI_IN_EYEBUTTON         RA5                                    // LBI
#define LBI_EYEBUTTON_ISDOWN()   (LBI_IN_EYEBUTTON == 0)                // LBI Active Low


// LED Colors
#define RED              0x02
#define GREEN            0x01
#define BLUE             0x04
#define TEAL             0x05
#define YELLOW           0x03
#define PURPLE           0x06
#define WHITE            0x07


// Uses BoardCheck For Some Pins
#include <BoardCheck.h>


// Turn Off After 10 Minutes Of No Use (LIKE STOCK FIRMWARE)
// Hold the power button for 1 and 1/2 seconds to turn the marker on.
// The blue LED in the grip will illuminate during the boot sequence.
// MUST FIND EYE POWER LIKE IN WAS CHIP SHEET (Eye power effects battery life and if to low debree will get in the way)

// 1000, 4000 = 1.30DCV (LED Brightness)
// ??, ?? = 0.05DCV (Eye Power)


#endif // _DM4MACROS_H
