/*
   ----------------------
   CustomDM4.c
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
#define COLOR_PAINTINBREACH         GREEN
#define COLOR_NOPAINTINBREACH       RED
#define COLOR_EYESOFF               RED


#include <CustomDM4.h>
#include <Algorithms.h>


__CONFIG(WDTEN & PWRTDIS & MCLRDIS & BORDIS & LVPDIS & PROTECT & INTIO);


volatile DWORD G_TickCount;                       // Timer In Microseconds
volatile BOOL G_Quit;                             // TRUE = ShutDown, FALSE = Running
volatile BYTE G_Mode = MODEID_FIRE;               //
BYTE G_Mode_Choose;                               // FALSE = No Mode Selected, TRUE = Mode Selected, 2 = Mode Just Selected
DWORD G_Last_Action;                              //
WORD G_Debounce;                                  // Minimum Trigger Release Time    (STORED IN EEPROM)
WORD G_Dwell;                                     // How Long The Bolt Stays Forward (STORED IN EEPROM)
BYTE G_ROF;                                       // Rate Of Fire                    (STORED IN EEPROM)
WORD G_AMB;                                       // Anti-Mechanical Bounce          (STORED IN EEPROM)
WORD G_BIP;                                       // Ball In Place Delay             (STORED IN EEPROM)
BYTE G_FMD;                                       // Fire Mode                       (STORED IN EEPROM)
bank1 volatile BOOL G_LED_Off;                    // TRUE = The LED Has Been Turned Off
bank1 volatile BOOL G_LED_Bright;                 // TRUE = The LED Is Bright Not Dimed
bank1 volatile DWORD G_LED_DelayOnTime;           // For LED Blinking Pattern
bank1 volatile DWORD G_LED_DelayOffTime;          // For LED Blinking Pattern
bank1 DWORD G_LED_NextBlink;                      // For LED Blinking Pattern
bank1 BOOL G_Eye_On;                              // TRUE = The Eye Is On
bank1 volatile BOOL G_Eye_Malfunc;                // TRUE = There Has Been A Eye Malfunction Because It Reads The Ball Hasnt Left The Chamber After The Shot
bank1 volatile BOOL G_Eye_Waiting;                // TRUE = Waiting For Bolt
bank1 DWORD G_Eye_NextCheck;                      // The Next Time A Eye Pulse Should Accure (Used For Adjusting Eye Power)
bank1 DWORD G_PowerButton_FP;                     // Power Button First Pressed
bank1 DWORD G_EyeButton_FP;                       // Eye Button First Pressed
bank1 BYTE G_BPS;                                 // Current BPS
bank1 BYTE G_Fire_RGB;                   // The RGB For The LED While In Firing Mode
bank1 volatile BYTE G_Current_ROF;                         // The Current ROF The Software May Have Changed During Use


/*
   Disable Interrupts Before Reading/Changing
   G_TickCount
   G_Eye_Waiting
   G_LED_Off
   G_LED_DelayOnTime
   G_LED_DelayOffTime
   G_Current_ROF
   G_Eye_Malfunc
   G_LimitROF
 */

/*
   Disable Interrupts Before Changing
   G_Quit
   G_Mode
   G_LED_Bright
   G_LED_Off
   G_LED_DelayOnTime
   G_LED_DelayOffTime
   G_Current_ROF
   G_Eye_Malfunc
   G_LimitROF
 */

/* @Desc: Report A Action To Reset POWERSAVE Counter
 */
void ACTION(void)
{
	DI();
	G_Last_Action = G_TickCount;
	EI();
	NOP();
}


/* @Desc: Set Back To Normal State
 */
void CLEAREYEMALFUNC(void)
{
	G_LED_Off = FALSE;
	G_LED_DelayOnTime = NULL;
	G_LED_DelayOffTime = NULL;
	G_Eye_Malfunc = FALSE;
	G_Current_ROF = G_ROF;
	G_LimitROF = DIP3_ISSET();
}


/* @Desc: Resets All Of The Timing Variables
 */
void CLEARTIMER(void)
{
	// Clear Timer Variables
	G_TickCount = 0;
	G_Last_Action = 0;
	G_LED_DelayOnTime = 0;
	G_LED_DelayOffTime = 0;
	G_LED_NextBlink = 0;
	G_Eye_NextCheck = 0;
	G_PowerButton_FP = 0;
	G_EyeButton_FP = 0;
	G_Last_Shot = 0;
	G_Next_Shot = 0;
	G_Trigger_FP = 0;
	G_Last_Pull = 0;
}


/* @Desc: Toggle To Next Mode
 */
void NEXTMODE(void)
{
	if (G_Mode < MODEID_COUNT-1)
	{
		DI();
		G_Mode++;
		EI();
		NOP();
	}
	else
	{
		DI();
		G_Mode = MODEID_FIRE+1;
		EI();
		NOP();
	}
}


/* @Desc: Cycle Through LED Colors
 */
void COLORCYCLE(void)
{
	BLINK_FAST(0x01);
	BLINK_FAST(0x04);
	BLINK_FAST(0x02);
}


/* @Desc: Cycle Through Colors Edit Mode Style
 */
void EDITMODECYCLE(void)
{
	LEDOFF();
	COLORCYCLE();
	COLORCYCLE();
	COLORCYCLE();
	COLORCYCLE();
	wait(TICK_SECOND);
}


/* @Desc: Turn All Of The LEDs Off
 */
void LEDOFF(void)
{
	OUT_LEDRED = OUT_LEDGREEN = OUT_LEDBLUE = 0;
}


/* @Desc: Set The LED Color
 */
void LEDCOLOR(BYTE RGB)
{
	OUT_LEDGREEN = ((0x01&RGB));
	OUT_LEDRED = ((0x02&RGB) == 0x02);
	OUT_LEDBLUE = ((0x04&RGB) == 0x04);
}


/* @Desc: Blink The RGB Color
 */
void BLINK(BYTE RGB)
{
	// Blink
	LEDCOLOR(RGB);
	wait(180000);
	LEDOFF();
	wait(350000);
}


/* @Desc: Quickly Blink The RGB Color
 */
void BLINK_FAST(BYTE RGB)
{
	// Blink
	LEDCOLOR(RGB);
	wait(50000);
	LEDOFF();
	wait(5000);
}


/* @Desc: Write A WORD To The EEPROM
 */
void eeprom_write_word(BYTE addr, WORD val)
{
	eeprom_write(addr, (/*0xFF&*/ (val>>8)));
	eeprom_write(addr+1, (/*0xFF&*/ val));
}


/* @Desc: Read A WORD From The EEPROM
   @Return: The WORD Read From The EEPROM
 */
WORD eeprom_read_word(BYTE addr)
{
	return ((eeprom_read(addr)<<8) | eeprom_read(addr+1));
}

/* @Desc: Toggle Input Bit And Check If Pulled Low Also Sets To Input So Interrupt Can Accur
   @Return: TRUE If Powerbutton Is Down
 */
BOOL T_POWERBUTTON_ISDOWN(void)  // Active Low (Toggle Input Bit)
{
	TRISB6 = 0; IN_POWERBUTTON = 1; TRISB6 = 1; return (IN_POWERBUTTON == 0);
}


/* @Desc: Interrupt Handler (Timer0/EYE/PowerButtonWakup)
   @Return:
 */
void interrupt INT(void)
{
	// Timer0 (8-BIT Timer)
	if (T0IF)
	{
		G_TickCount += 255; // 255 MicroSeconds
		TMR0 -= 255;
		T0IF = NULL; // Clear Interrupt Flag
	}

	// Timer1 (16-BIT Timer)
	if (TMR1IF)
	{
		if (!G_Quit && (G_Mode == MODEID_FIRE))
		{
			if ((OUT_LEDGREEN || OUT_LEDRED || OUT_LEDBLUE) && !G_LED_Bright)
			{
				LEDOFF();
				TMR1L = 0x5F; //(0xFF00&(0xFFFF-4000));
				TMR1H = 0xF0; //(0x00FF&(0xFFFF-4000));
			}
			else if (!G_LED_Off)
			{
				OUT_LEDGREEN = ((0x01&G_Fire_RGB)); //EYEINPUT_ISBROKE();
				OUT_LEDRED = ((0x02&G_Fire_RGB) == 0x02); //!EYEINPUT_ISBROKE();
				OUT_LEDBLUE = ((0x04&G_Fire_RGB) == 0x04);
				TMR1L = 0x17; //(0xFF00&(0xFFFF-1000));
				TMR1H = 0xFC; //(0x00FF&(0xFFFF-1000));
			}
		}
		TMR1IF = NULL; // Clear Interrupt Flag
	}

	// External Interrupt RB4-RB7 (PowerButton)
	if (RBIF)
	{
		NOP();
		if (TRIGGER_ISDOWN())
			LEDOFF();
		RBIF = NULL;
	}

	// External Interrupt RB0 (Eye)
	if (G_Eye_Waiting)
	{
		// Watch For Bolt Back
		if (!EYEINPUT_ISBROKE())
		{
			// Bolt All The Way Back And Ready For Next Shot
			G_Eye_Waiting = FALSE;
			// Reset Eye Malfunction
			if (G_Eye_Malfunc)
			{
				CLEAREYEMALFUNC();
			}
		}
	}
}


/* @Desc: Wait For A Amount Of Time
   @microseconds: The Amount Of Time To Wait In Microseconds
   @Return:
 */
void wait(DWORD microseconds)
{
	static bank1 DWORD end;

	DI();
	end = G_TickCount + microseconds;
	while (G_TickCount < end)
	{
		EI();
		NOP();
		KICKDOG();
		DI();
	}
	EI();
	NOP();
}


/* @Desc: Fire Mode Process
   @Return:
 */
void Mode_Fire(void)
{
	static bank1 BOOL shotforced = TRUE;
	DWORD end;

	// Check For Pull
	if (TRIGGER_FIRSTPULL())
	{
		// Add Shot To Shot Buffer
		G_ShotBuffer = 1;

		// If In PSP Mode Fire 3
		if ((G_Hybrid == TRUE) && (G_FMD == FIREMODE_PSP))
		{
			G_ShotBuffer = 3;
		}

		// Force Shot
		shotforced = FALSE;

		// Green LED Flash While Eyes Disabled
		if (!G_Eye_On)
		{
			// Flash Green
			G_Fire_RGB = GREEN|COLOR_EYESOFF;
			LEDGREEN_ON();
			LEDRED_ON();
			wait(500);
			LEDGREEN_OFF();
			LEDRED_OFF();
			G_Fire_RGB = COLOR_EYESOFF;
		}
	}

	// Keep This Space Between Check For Pull And Check For Cycle Empty For Optomized Speed

	// Cycle Marker
	if (G_Eye_On == TRUE)
	{
		// Pulse Eye Voltage/Cycle Check To Save Battery
		DI();
		if ((G_TickCount > G_Eye_NextCheck) || G_ShotBuffer) // Check Eye 500 Times A Second To Decrease Voltage
		{
			EI();
			NOP();
			// Eyes Need To Be On From Here On (Code That Needs Eyes In Here)
			EYEPOWER_ON(); // Eyes Need To Be On From Here On (Code That Needs Eyes In Here)

			// Calculate Next Check
			DI();
			G_Eye_NextCheck = G_TickCount+(TICK_SECOND/125);
			EI();
			NOP();

			// Wait For IR Sensor
			wait(20);

			// Watch For Eye Malfunction!!
			DI();
			if (G_Eye_Waiting)
			{
				if (G_TickCount > G_Last_Shot+(TICK_SECOND/15))
				{
					EI();
					NOP();
					// Eye Malfunction Accured
					DI();
					if (G_Eye_Malfunc == FALSE)
					{
						EI();
						NOP();
						DI();
						G_LED_DelayOnTime = (TICK_SECOND/4);
						G_LED_DelayOffTime = (TICK_SECOND/8);
						G_Eye_Malfunc = TRUE;
						G_Current_ROF = 10;
						G_Ramping = G_RampIt = FALSE;
						G_LimitROF = TRUE;
						EI();
						NOP();
					}
					EI();
					NOP();

					DI();
					G_Eye_Waiting = FALSE;
					EI();
					NOP();
				}
			}
			EI();
			NOP();

			// Update The LED Color For Fire Mode
			if (EYEINPUT_ISBROKE())
			{
				DI();
				G_Fire_RGB = COLOR_PAINTINBREACH; // Paint In Breach
				EI();
				NOP();
			}
			else
			{
				DI();
				G_Fire_RGB = COLOR_NOPAINTINBREACH; // No Paint In
				EI();
				NOP();
				// Watch For Chance For Eye Malfunction FIX!!
				DI();
				if (G_Eye_Malfunc)
				{
					EI();
					NOP();
					CLEAREYEMALFUNC();
				}
				EI();
				NOP();
			}

			// If Shot Cycle Marker
			if (G_ShotBuffer)
			{
				Cycle_BreakBeam();
			}

			// Check For Force Fire
			DI();
			if ((G_FMD != FIREMODE_NXL) && TRIGGER_ISDOWN() && !EYEINPUT_ISBROKE() && !shotforced && (G_TickCount > G_Last_Pull+(TICK_SECOND/4)))
			{
				EI();
				NOP();
				DI();
				end = G_TickCount+(TICK_SECOND/2);
				while (G_TickCount < end)
				{
					EI();
					NOP();
					KICKDOG();
					if (!TRIGGER_ISDOWN() || EYEINPUT_ISBROKE())
					{
						shotforced = TRUE; // Do Not Shoot
						end = NULL;
					}
					DI();
				}
				EI();
				NOP();

				// One Single Cycle Without Break Beam Eyes
				if (!shotforced)
				{
					// Cycle Marker (Turn Solenoid On And Off)
					SOLENOID_ON();
					wait(G_Dwell); // Wait For The Amount Of Dwell
					SOLENOID_OFF();
				}

				shotforced = TRUE;
			}
			EI();
			NOP();

			// Eyes Need To Be Off From Here On (Code That Needs Eyes In Here)
			DI();
			if (G_Eye_Waiting == FALSE) // Keep Eyes On While There Is A Eye Malfunction
			{
				EI();
				NOP();
				EYEPOWER_OFF(); // Eyes Need To Be Off From Here On
			}
			EI();
			NOP();
		}
		EI();
		NOP();
	}
	else // Eye Off
	{
		if (G_ShotBuffer)
		{
			Cycle_BreakBeam(); // Shoot Gun Normaly
		}
	}

	// Blink LED For Fire Mode States
	DI();
	if ((G_TickCount > G_LED_NextBlink) && G_LED_DelayOnTime && G_LED_DelayOnTime)
	{
		G_LED_Off = !G_LED_Off;

		if (G_LED_Off == TRUE)
			G_LED_NextBlink = G_TickCount+G_LED_DelayOffTime;
		else
			G_LED_NextBlink = G_TickCount+G_LED_DelayOnTime;
	}
	EI();
	NOP();

	// EyeButton
	if (EYEBUTTON_ISDOWN())
	{
		if (G_EyeButton_FP == NULL)
		{
			DI();
			G_EyeButton_FP = G_TickCount;
			EI();
			NOP();
		}

		DI();
		if (G_TickCount > G_EyeButton_FP+TICK_SECOND)
		{
			EI();
			NOP();
			G_EyeButton_FP = NULL;
			CLEAREYEMALFUNC();
			G_Eye_On = !G_Eye_On;

			if (G_Eye_On == FALSE)
			{
				OUT_EYEPOWER = 0;
				G_LED_DelayOnTime = (TICK_SECOND/4);
				G_LED_DelayOffTime = (TICK_SECOND/6);
				DI();
				G_Fire_RGB = COLOR_EYESOFF; // red
				EI();
				NOP();
			}
			else
			{
				OUT_EYEPOWER = 1;
			}
		}
		EI();
		NOP();
	}
	else
		G_EyeButton_FP = NULL;
}


/* @Desc: Used To Select Settings Of Each Mode
   @var: Current Value Of The Mode Setting
   @min: The Minimum Value Of The Mode Setting
   @max: The Maximum Value Of The Mode Setting
   @inc: The Value To Increment var By
   @RGB: The Color That Represents The Mode
   @Return: The New Value Of var
 */
WORD Mode_Proc(WORD var, WORD min, WORD max, WORD inc, BYTE RGB)
{
	BYTE x;
	BYTE mincount = min/inc;
	BYTE varcount = (var/inc)+1;
	WORD premin = min-inc;

	if (G_Mode_Choose == FALSE)
	{
		LEDCOLOR(RGB);
	}
	else if (G_Mode_Choose == 2)
	{
		// Show Current Setting
		for (x = mincount; x <= varcount; x++)
		{
			// Blink
			BLINK(RGB);
			if (!TRIGGER_ISDOWN()) break; // Only Show Setting While Trigger Is Down (NOT LIKE STOCK CHIP)
		}
		var = premin;
		G_Mode_Choose = TRUE;
		ACTION();
	}
	else if (G_Mode_Choose == TRUE)
	{
		// Watch For Pull/Release
		if (TRIGGER_FIRSTPULL())
		{
			// Increase Setting/Trigger Pressed
			var += inc;
			G_Trigger_FP = TRUE;
			ACTION();

			// Limit Value
			if (var > max)
			{
				var = max; // Limit var

				// This Is Not Like The Stock Chip!!!
				EDITMODECYCLE(); // Cycle Colors
				G_Mode_Choose = FALSE; // Reset The Choose Mode So They Can Choose Another Setting Mode To Edit
			}
		}
	}

// IDLE Till First Press
	if (var == premin)
		ACTION();

	return var;
}


/* @Desc: Watch For A Need To Change Modes
   @Return:
 */
void ChangeModeProc(void)
{
	if (!G_Mode_Choose)
	{
		// Toggle Modes
		if (TRIGGER_ISUP(G_Debounce))
		{
			if (G_Trigger_FP)
			{
				NEXTMODE(); // Toggle To Next Mode

				G_Trigger_FP = NULL;
			}
		}
		else
		{
			if (!G_Trigger_FP)
			{
				DI();
				G_Trigger_FP = G_TickCount;
				EI();
				NOP();
			}

			// Trigger Held Down More Than A Second
			DI();
			if (G_TickCount > G_Trigger_FP+TICK_SECOND)
			{
				EI();
				NOP();
				G_Mode_Choose = 2;
			}
			EI();
			NOP();
		}
	}
	else
	{
		// Done Editing
		DI();
		if (G_TickCount > G_Last_Action+(TICK_SECOND*2))
		{
			EI();
			NOP();
			EDITMODECYCLE();
			G_Mode_Choose = FALSE;
			G_Trigger_FP = NULL;
		}
		EI();
		NOP();
	}

}


/* @Desc: Perform Boot Sequence, Power Board, Init Timers, etc
   @Return:
 */
void StartUp(void)
{
	BYTE version;
	BYTE checksum;

	// Disable Interrupts
	DI();

	// Setup Ports
	CMCON = 0x07; //
	_CHOOSEBOARD(); // Check Board Type/Point Function Pointers To The Right Functions/Port Configuration

	// Setup Timer0
	T0CS = 0;    // Increments On Instruction Clock
	T0IE = 1;    // Enable Interrupt On Timer 0 Overflow
	TMR0 = 0;

	// Setup Timer 1
	TMR1IE = 1;
	TMR1ON = 1;
	TMR1L = 0x17; //(0xFF00&(0xFFFF-1000));
	TMR1H = 0xFC; //(0x00FF&(0xFFFF-1000));

	// External Interrupt
	RBIE = 0;                   // Disable The RB4-RB7 External Interrupt (PowerButton)
	INTE = 0;                   // Disable The RB0 External Interrupt (Eye)
	INTEDG = 0;                 // Trigger Eye Interrupt On Falling Edge
	//RBPU = 0;                         // PORTB Internal Pullup

	// Default Variable
	G_Eye_On = TRUE;
	G_Mode = MODEID_FIRE;
	G_LimitROF = DIP3_ISSET();
	CLEAREYEMALFUNC(); // LED On, No Blinking, No Eye Malfunction

	// Clear Timer Variables
	CLEARTIMER();

	// Enable Global Interrupt
	EI();        // Global Interrupt Enable
	NOP();

	if (G_Quit == TRUE)
	{
		// Bootup Sequence
		LEDBLUE_ON();
		wait(HALF_SECOND*3);
		LEDBLUE_OFF();

		// Check For Edit Mode/Check Configuration
		if (TRIGGER_ISDOWN())
		{
			if (DIP4_ISSET())
			{
				EDITMODECYCLE();

				DI();
				G_Mode = MODEID_FIRE+1; // Shift Mode To A Edit Mode
				EI();
				NOP();
			}
			else
			{
				// Check Battery Voltage (FAKE Eye Sensitivity)
				_CHECKBATTERY();

				// ABS On/Off
				if (DIP1_ISSET())
				{
					BLINK(BLUE);
				}
				BLINK(BLUE);
			}
		}

		DI();
		G_Quit = FALSE;
		EI();
		NOP();
	}

	// Read EEPROM Settings
	version = eeprom_read(ADDR_VERSION);
	checksum = eeprom_read(ADDR_CHKSUM);
	G_Debounce = eeprom_read_word(ADDR_DEBOUNCE);
	G_Dwell = eeprom_read_word(ADDR_DWELL);
	G_ROF = eeprom_read(ADDR_ROF);
	G_AMB = eeprom_read_word(ADDR_AMB);
	G_BIP = eeprom_read_word(ADDR_BIP);
	G_FMD = eeprom_read(ADDR_FMD);

	// EEPROM Verify Settings With CheckSum
	if ((checksum != 0x100-(0xFF&(VERSION+(0x00FF&G_Dwell)+G_ROF))) || (version != VERSION))
	{
		G_Debounce = DEFAULT_DEBOUNCE;
		G_Dwell = DEFAULT_DWELL;
		G_ROF = DEFAULT_ROF;
		G_AMB = DEFAULT_AMB;
		G_BIP = DEFAULT_BIP;
		G_FMD = DEFAULT_FMD;
	}

	DI();
	G_Current_ROF = G_ROF; // Update Current ROF
	EI();
	NOP();

	G_ShotBuffer = 0;
	G_Trigger_FP = 1;
}


/* @Desc: Wait For A Amount Of Time
   @Return:
 */
void ShutDown(void)
{
  #ifndef RAMPING // If Ramping is not defined
	if (G_FMD == FIREMODE_RAMP)
		G_Ramping = TRUE;
	else
		G_Ramping = FALSE; // No Ramping when it is turned off
  #endif

	// Save Settings
	if (G_Mode != MODEID_FIRE)
	{
		// Write EEPROM Settings
		eeprom_write(ADDR_VERSION, VERSION);
		eeprom_write(ADDR_CHKSUM, 0x100-(0xFF&(VERSION+(0x00FF&G_Dwell)+G_ROF)));
		eeprom_write_word(ADDR_DEBOUNCE, G_Debounce);
		eeprom_write_word(ADDR_DWELL, G_Dwell);
		eeprom_write(ADDR_ROF, G_ROF);
		eeprom_write_word(ADDR_AMB, G_AMB);
		eeprom_write_word(ADDR_BIP, G_BIP);
		eeprom_write(ADDR_FMD, G_FMD);
	}

	// Output Signs Of TurnOff
	LEDOFF();
	G_Eye_On = FALSE;

	// Wait For Them To Release PowerButton
	DI();
	G_Quit = TRUE;
	G_LED_Off = TRUE; // LED Off
	G_Fire_RGB = 0x00;

	while (T_POWERBUTTON_ISDOWN())
	{
		EI();
		NOP();
		KICKDOG();
		PORTB = 0x00; // Go Ahead For Power And Solenoid Safety
		DI();
	}
	EI();
	NOP();

	// So MCU Can Be Woken From Sleep
	RBIE = 1; // Enable The RB4-RB7 External Interrupt (PowerButton)

	T_POWERBUTTON_ISDOWN(); // Sets Powerbutton To Input

	// SavePower And Wait For Interrupt
	_POWERSAVE();
	SLEEP();
}


/* @Desc: MAIN!!!!
   @Return: 0 If Successful
 */
void main()
{
	// Default When Chip Is Placed In Dip Socked (Initialization Without Bootup Sequence Then Goes To Sleep Till Woken Up)
	StartUp();
	ShutDown();

	while(TRUE)
	{
		if (G_Quit == FALSE) // If The Marker Is Turned On
		{
			// Break Out Of Edit Mode
			if (G_Mode != MODEID_FIRE)
			{
				switch (G_Mode)
				{
				case MODEID_DEBOUNCE:
					G_Debounce = Mode_Proc(G_Debounce, MIN_DEBOUNCE, MAX_DEBOUNCE, INC_DEBOUNCE, 0x01);
					break;

				case MODEID_DWELL:
					G_Dwell = Mode_Proc(G_Dwell, MIN_DWELL, MAX_DWELL, INC_DWELL, 0x02);
					break;

				case MODEID_ROF:
					G_ROF = Mode_Proc(G_ROF, MIN_ROF, MAX_ROF, INC_ROF, 0x04);
					break;

				case MODEID_AMB:
					if (DIP2_ISSET())
						G_AMB = Mode_Proc(G_AMB, MIN_AMB, MAX_AMB, INC_AMB, 0x05);
					else
						NEXTMODE();
					break;

				case MODEID_BIP:
					G_BIP = Mode_Proc(G_BIP, MIN_BIP, MAX_BIP, INC_BIP, 0x03);
					break;

				case MODEID_FMD:
					G_FMD = Mode_Proc(G_FMD, MIN_FMD, MAX_FMD, INC_FMD, 0x06);
					break;
				}

				ChangeModeProc();
			}
			else // Normal Operation
			{
				// Fire Mode
				Mode_Fire();
			}
		}

		// PowerButton (TurnOn/TurnOff)
		if (T_POWERBUTTON_ISDOWN())
		{
			if (G_PowerButton_FP == NULL)
			{
				DI();
				G_PowerButton_FP = G_TickCount;
				EI();
				NOP();
			}

			DI();
			if (G_TickCount > G_PowerButton_FP+(HALF_SECOND*3))
			{
				EI();
				NOP();
				G_PowerButton_FP = NULL;

				// Startup Or Shutdown
				DI();
				if (G_Quit)
				{
					EI();
					NOP();
					// StartUp
					StartUp();
				}
				else
				{
					EI();
					NOP();
					// Save Settings N ShutDown
					ShutDown();
				}
				EI();
				NOP();
			}
			EI();
			NOP();
		}
		else
		{
			G_PowerButton_FP = NULL;

			if (G_Quit) // If Power Button Released After PowerDown Go Back To Sleep
			{
				ShutDown();
			}
		}

		// 10min Power SAVE
		DI();
		if (G_TickCount > G_Last_Action+(TICK_SECOND*600))
		{
			EI();
			NOP();
			// Record Action
			ACTION();
			// ShutDown
			ShutDown();
		}
		EI();
		NOP();

		// KEEP TIMER FROM OVERFLOWING
		DI();
		if (G_TickCount > ((TICK_SECOND*600)*3))
		{
			EI();
			NOP();
			// Restart Timer
			CLEARTIMER();
		}
		EI();
		NOP();

		// Kick Dog
		KICKDOG();
	}
}
