/*
   ----------------------
   CustomDM4.h
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
#ifndef _CUSTOMDM4_H
#define _CUSTOMDM4_H


#include <DM4Macros.h>


/* Version
 #0 David's Personal Release
 #1 Custom Done Firmware Release
 #2 V1 Beta Release
 #3 V1 Official Release
 #4 V1.1.3 UV Sensitive Bug Fix
 #5 V1.1.4 New % Shot Ramping And Parabolic Ramping Algorithm
 #6 V1.1.5 Fixed Eye Malfunction Cap
 #7 V1.2 Works In Both NON-LBI and LBI boards
 #8 V1.2.1 Fixed Extra Shot During Turn On
 #9 V1.3 Works with PICC v9.x.x, one file for code and non-code version, Extra Shot During Turn On Bug Fixed Again
 */
#define VERSION 0x09               // Version Of Software

// MIN/MAX/DEFAULT Settings
#define MIN_DEBOUNCE      0        // 0ms
#define MAX_DEBOUNCE      60000    // 60ms
#define INC_DEBOUNCE      1000     // 1ms
#define DEFAULT_DEBOUNCE  5000     // 5ms
#define MIN_AMB           1000     // 1ms
#define MAX_AMB           40000    // 40ms
#define INC_AMB           1000     // 1ms
#define DEFAULT_AMB       10000    // 10ms
#define MIN_DWELL         1000     // 1ms
#define MAX_DWELL         35000    // 35ms
#define INC_DWELL         1000     // 1ms
#define DEFAULT_DWELL     18000    // 18ms
#define MIN_ROF           11       // 11bps
#define MAX_ROF           25       // 25bps
#define INC_ROF           1        // 1bps
#define DEFAULT_ROF       20       // 20bps
#define MIN_BIP           500      // 1/2ms
#define MAX_BIP           20000    // 20ms
#define INC_BIP           500      // 1/2ms
#define DEFAULT_BIP       1000     // 1ms
#define MIN_FMD           1        // Mode 1
#define MAX_FMD           3        // 3 Modes
#define INC_FMD           1        // 1 Mode
#define DEFAULT_FMD       1        // Mode 1

#define ADDR_VERSION     0x00
#define ADDR_CHKSUM      0x01
#define ADDR_DEBOUNCE    0x02
#define ADDR_DWELL       0x04
#define ADDR_ROF         0x06
#define ADDR_AMB         0x07
#define ADDR_BIP         0x09
#define ADDR_FMD         0x0B

#define MODEID_FIRE      0x00
#define MODEID_DEBOUNCE  0x01
#define MODEID_DWELL     0x02
#define MODEID_ROF       0x03
#define MODEID_AMB       0x04
#define MODEID_BIP       0x05
#define MODEID_FMD       0x06
#define MODEID_COUNT     0x07

#define TICK_SECOND      1000000
#define HALF_SECOND      500000

#define FIREMODE_SEMI    0x01
#define FIREMODE_NXL     0x02
#define FIREMODE_PSP     0x03
#define FIREMODE_RAMP    0x04
#undef MAX_FMD                     // Redefine MAX Modes
#undef DEFAULT_FMD                 // Redefine Default Mode
#define MAX_FMD           4        // 3+1 Modes
#define DEFAULT_FMD       4        // Mode 4


// Global Variables
extern volatile DWORD G_TickCount;
extern volatile BOOL G_Quit;
extern volatile BYTE G_Mode;
extern BYTE G_Mode_Choose;
extern DWORD G_Last_Action;
extern WORD G_Debounce;
extern WORD G_Dwell;
extern BYTE G_ROF;
extern WORD G_AMB;
extern WORD G_BIP;
extern BYTE G_FMD;
extern volatile bank1 BOOL G_LED_Off;
extern bank1 DWORD G_LED_NextBlink;
extern volatile bank1 BOOL G_LED_Bright;
extern volatile bank1 DWORD G_LED_DelayOnTime;
extern volatile bank1 DWORD G_LED_DelayOffTime;
extern bank1 BOOL G_Eye_On;
extern volatile bank1 BOOL G_Eye_Malfunc;
extern volatile bank1 BOOL G_Eye_Waiting;
extern bank1 DWORD G_Eye_NextBlink;
extern bank1 DWORD G_PowerButton_FP;
extern bank1 DWORD G_EyeButton_FP;
extern bank1 BYTE G_BPS;
extern bank1 BYTE G_Fire_RGB;
extern volatile bank1 BYTE G_Current_ROF;
extern bank1 BOOL G_Shot_Forced;


// CustomDM4 Helping Functions
void ACTION(void);
void CLEAREYEMALFUNC(void);
void CLEARTIMER(void);
void NEXTMODE(void);
void COLORCYCLE(void);
void EDITMODECYCLE(void);
void LEDOFF(void);
void LEDCOLOR(BYTE RGB);
void BLINK(BYTE RGB);
void BLINK_FAST(BYTE RGB);
void eeprom_write_word(BYTE addr, WORD val);
WORD eeprom_read_word(BYTE addr);
BOOL T_POWERBUTTON_ISDOWN(void);

// CustomDM4 Functions
void INT(void);
void wait(DWORD microseconds);
void Mode_Fire(void);
WORD Mode_Proc(WORD var, WORD min, WORD max, WORD inc, BYTE RGB);
void ChangeModeProc(void);
void StartUp(void);
void ShutDown(void);
void main();


#endif // _CUSTOMDM4_H
