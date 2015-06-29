/*
   ----------------------
   BoardCheck.h
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
#ifndef _BOARDCHECK_H
#define _BOARDCHECK_H


#include <DM4Macros.h>


#define NONLBIBOARD 0x00
#define LBIBOARD    0x01

#define FULLBATTERY 4

typedef BOOL (*LBI_FUNCTION_GET)(void);
typedef void (*LBI_FUNCTION_SET)(void);

extern BYTE G_Board_Type;
extern BYTE G_Board_Battery;
extern bank1 LBI_FUNCTION_GET _DIP1_ISSET_PTR;
extern bank1 LBI_FUNCTION_GET _EYEBUTTON_ISDOWN_PTR;
extern bank1 LBI_FUNCTION_SET _EYEPOWER_ON_PTR;
extern bank1 LBI_FUNCTION_SET _EYEPOWER_OFF_PTR;


BOOL _NONLBI_DIP1_ISSET(void);
BOOL _LBI_DIP1_ISSET(void);

BOOL _NONLBI_EYEBUTTON_ISDOWN(void);
BOOL _LBI_EYEBUTTON_ISDOWN(void);

void _NONLBI_EYEPOWER_ON(void);
void _LBI_EYEPOWER_ON(void);
void _NONLBI_EYEPOWER_OFF(void);
void _LBI_EYEPOWER_OFF(void);

void _CHOOSEBOARD(void);
void _COMPARE(BYTE vr);
void _CHECKBATTERY(void);
void _POWERSAVE(void);


#endif // _BOARDCHECK_H
