/*
   ----------------------
   Algorithms.c
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
#define RAMP_PERCENT       0.60f  // %
#define RAMP_ACTIVATION    8      // bps
#define RAMP_DEACTIVATION  5      // bps


#include <Algorithms.h>


bank1 BYTE G_ShotBuffer;
bank1 DWORD G_Last_Shot;
bank1 DWORD G_Next_Shot;
bank1 DWORD G_Trigger_FP;                         // The Trigger Was Up
bank1 BOOL G_LimitROF;
bank1 DWORD G_Last_Pull;
bank1 BOOL G_Hybrid;                             // Hybrid LEGAL Semi (NXL,PSP)
bank1 BOOL G_Ramping;
bank1 BOOL G_RampIt;


/* @Desc: Check For A True Release With Debounce
   @Return: TRUE If The Trigger Was Released
 */
BOOL TRIGGER_ISUP(WORD debounce)
{
	DWORD end;

	if (TRIGGER_ISDOWN() == FALSE)
	{
		DI();
		end = G_TickCount+debounce;
		while (G_TickCount < end)
		{
			EI();
			NOP();
			KICKDOG();
			if (TRIGGER_ISDOWN()) return FALSE;
			DI();
		}
		EI();
		NOP();
		return TRUE;
	}
	return FALSE;
}


/* @Desc: Check For A True Trigger Pull
   @Return: TRUE If The Trigger Was Pressed
 */
BOOL TRIGGER_FIRSTPULL(void)
{
	static bank1 WORD ambdebounce; // AMB
	static bank1 BYTE ambcount;  // AMB
	static bank1 BYTE hybridcount;
	static bank1 float bps;      // Floating Point Accurate BPS
	static bank1 float ramppercent;
	static bank1 BYTE rampbps;

	// Check For Pull/Release
	if (G_Trigger_FP)
	{
		if (!DIP2_ISSET()) // NO AMB
		{
			// Check For Release With NO AMB
			if (TRIGGER_ISUP(G_Debounce))
			{
				G_Trigger_FP = NULL;
			}
		}
		else // WITH AMB
		{
			// Check For Release With AMB
			if (TRIGGER_ISUP(ambdebounce))
			{
				G_Trigger_FP = NULL;
			}
		}
	}
	else if (TRIGGER_ISDOWN())
	{
		ACTION();

		// Record The First Pull
		G_Trigger_FP = 1;

		// Record BPS
		DI();
		bps = (float)(TICK_SECOND/(G_TickCount-G_Last_Pull));
		EI();
		NOP();
		G_BPS = (BYTE)(bps);

		// Record The Last PULL
		DI();
		G_Last_Pull = G_TickCount;
		EI();
		NOP();

		// AMB Algorithm
		if (DIP2_ISSET())
		{
			if (G_BPS < 8)
			{
				ambcount = NULL;
				ambdebounce = ((WORD)(G_Debounce+G_AMB)); // Need AMB
			}
			else
			{
				ambcount++;
				if (ambcount > 1)
				{
					ambdebounce = G_Debounce; // No Need For AMB Anymore
					ambcount = 2;
				}
			}
		}
		// --- AMB Algorithm

// Ramping Modes
		if (G_Mode == MODEID_FIRE)
		{
			// Enable Hybrid Modes
			if (G_Hybrid == FALSE)
			{
				if (G_BPS >= 5)
				{
					hybridcount++;
					if (hybridcount > 2) // shot1 --- shot2 --- shot 3  = 2 bps gaps
					{
						G_Hybrid = TRUE;
						hybridcount = 3;
					}
				}
				else
				{
					hybridcount = NULL;
				}
			}

			if (G_FMD == FIREMODE_RAMP)
			{
				// Calculate Ramping Values
				//ramppercent = (bps-7.00f)*0.100f;            // Parabolic Ramping
				rampbps = G_BPS + ((BYTE)(G_BPS*(RAMP_PERCENT))); // xx Is The Percentage To Ramp Your Current ROF (xx% Shot RAmping)

				if (G_RampIt == TRUE)
				{
					if (G_BPS >= (RAMP_DEACTIVATION+1)) // De-Activate At xxbps
					{
						G_Current_ROF = rampbps;
					}
					else
					{
						// RESTORE PURE SEMI
						G_RampIt = FALSE;

						// Restore Current ROF
						DI();
						G_Current_ROF = G_ROF;
						G_LimitROF = DIP3_ISSET();
						EI();
						NOP();
					}
				}
				else if (G_Ramping == TRUE)
				{
					// First Time Entered RAmping
					if (G_BPS >= RAMP_ACTIVATION) // Ramp At xxBPS
					{
						// START Ramping
						G_RampIt = TRUE;

						DI();
						G_Current_ROF = G_BPS; // Ramp Goal
						G_LimitROF = TRUE;
						EI();
						NOP();
					}
				}
				// If The Eyes Are Off Limit Ramping To The ROF
				if ((DIP3_ISSET() == TRUE) || (G_Eye_On == FALSE))
				{
					if (G_Current_ROF > G_ROF)
						G_Current_ROF = G_ROF;
				}
			}
		}
		return TRUE;
	}

// Hybrid Modes
	if (G_Hybrid == TRUE)
	{
		if (G_FMD == FIREMODE_NXL) // NXL FULlAUTO
		{
			if (TRIGGER_ISDOWN())
			{
				G_ShotBuffer = 2; // Keep A Extra Shot In Buffer
			}
			else
			{
				G_ShotBuffer = 0;
			}
		}

		// Let Idle For A Second Then Go Back Semi
		if (G_TickCount > G_Last_Shot+TICK_SECOND)
		{
			G_Hybrid = FALSE; // No More Hybrid Mode
			hybridcount = NULL; // Start Over
			G_ShotBuffer = NULL;
		}
	}

	if (G_Ramping && (G_RampIt == TRUE))
	{
		if ((G_TickCount-G_Last_Pull) < (TICK_SECOND/(G_BPS*2))) // Limit To 1 Extra Shot Per Pull
		{
			G_ShotBuffer = 1; // Maintain ROF
		}
	}

	return FALSE;
}


/* @Desc: Cycle Marker
   @Return:
 */
void Cycle(void)
{
	WORD dwell;

	// ABS (Anti-Bolt Stick) Algorithm
	DI();
	if (DIP1_ISSET() && ((G_TickCount > G_Last_Shot+(TICK_SECOND*8)) || !G_Last_Shot))
	{
		EI();
		NOP();
		dwell = MAX_DWELL; // Set Dwell To The Highest Setting To Loosen Bolt
	}
	else
	{
		EI();
		NOP();
		dwell = G_Dwell; // Set To Normal Dwell Settting
	}
	EI();
	NOP();
	// ABS ----

	// Calculate Next Shot
	DI();
	if ((G_LimitROF == TRUE) || (G_Eye_On == FALSE))
	{
		EI();
		NOP();
		DI();
		G_Next_Shot = G_TickCount+(TICK_SECOND/G_Current_ROF); // WITH ROF CAPP
		EI();
		NOP();
	}
	else
	{
		DI();
		EI();
		NOP();
		DI();
		G_Next_Shot = NULL; // If Eyes Are On Then No Need To Capp ROF
		EI();
		NOP();
	}
	EI();
	NOP();

	// Cycle Marker (Turn Solenoid On And Off)
	SOLENOID_ON();
	wait(dwell);     // Wait For The Amount Of Dwell
	SOLENOID_OFF();

	// Record Last Shot
	DI();
	G_Last_Shot = G_TickCount;
	EI();
	NOP();

	// Clear Shot From Shot Buffer
	G_ShotBuffer--;
}


/* @Desc: Cycle Marker
   @Return:
 */
void Cycle_BreakBeam(void)
{
	DWORD end;

// Make Sure It Isn't Shot Above The Maximum ROF
	DI();
	if (G_TickCount > G_Next_Shot)
	{
		EI();
		NOP();

		// Check Eye State
		if (G_Eye_On)
		{
			DI();
			if (EYEINPUT_ISBROKE() && !G_Eye_Waiting) // Ball In Breach And Eye Not Waiting
			{
				EI();
				NOP();
				// Wait For The Eye Delay
				DI();
				end = G_TickCount+G_BIP;
				while(end > G_TickCount)
				{
					EI();
					NOP();
					KICKDOG();
					if (!EYEINPUT_ISBROKE()) return; // Dont Shoot If Ball Isnt In Place For The BIP Delay
					DI();
				}
				EI();
				NOP();

				// Cycle Marker
				Cycle();

				// Compensate BIP Delay Into Next Shot Calculation
				if (G_Next_Shot)
				{
					G_Next_Shot -= G_BIP;
				}

				// Monitor Bolt
				EYEPOWER_ON(); // Turn Eye On Until Bolt Is Done Monitoring
				DI();
				G_Eye_Waiting = TRUE; // Waiting For Bolt
				EI();
				NOP();
			}
			else // No Ball In Breach
			{
				EI();
				NOP();
				// Store Shots Even While Ball Is Loading Unless Slower Than 2bps
				DI();
				if (G_TickCount > G_Last_Shot+(TICK_SECOND/2))
				{
					EI();
					NOP();
					G_ShotBuffer = NULL;
				}
				EI();
				NOP();
				// Store -----
			}
			EI();
			NOP();
		}
		else // Eyes Off
		{
			Cycle();
		}
	}
	EI();
	NOP();
}
