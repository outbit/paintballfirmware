/*
   ----------------------
   Algorithms.h
   ----------------------
   Created: 10/24/04
   Updated: 05/22/06
   Version: 1.3
   ----------------------
   Compiler: PICC 9.40 C Compiler From HI-TECH
   Assembler: PICC 9.40 Assembler From HI-TECH
   Linker: PICC Linker From HI-TECH
   IDE: MPLAB IDE v6.62
   ----------------------
   Author: David Lee Whiteside
   Company: Hyperformance Paintball LLC
   Project: CodeX DM4/5/C Chip(TM)

   Target MCU: PIC16F648a From Microchip
   ----------------------
   The CodeX(TM) software is (C) 2004-2009 Whiteside Solutions LLC.
   This program is free software; you may redistribute and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; Version 3.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License v3.0 for more details at http://www.gnu.org/licenses/gpl-3.0.html
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
