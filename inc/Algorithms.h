/*
   ----------------------
   Algorithms.h
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


#ifndef _ALGORITHMS_H
#define _ALGORITHMS_H


#include <DM4Macros.h>
#include <CustomDM4.h>


extern bank1 BYTE G_ShotBuffer;
extern bank1 DWORD G_Last_Shot;
extern bank1 DWORD G_Next_Shot;
extern bank1 DWORD G_Trigger_FP;                         // The Trigger Was Up
extern bank1 BOOL G_LimitROF;
extern bank1 DWORD G_Last_Pull;
extern bank1 BOOL G_Hybrid;
extern bank1 BOOL G_Ramping;
extern bank1 BOOL G_RampIt;


BOOL TRIGGER_ISUP(WORD debounce);
BOOL TRIGGER_FIRSTPULL(void);
void Cycle(void);
void Cycle_BreakBeam(void);


#endif // _ALGORITHMS_H
