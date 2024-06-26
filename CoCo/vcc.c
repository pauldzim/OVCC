/*
Copyright 2015 by Joseph Forgione
This file is part of VCC (Virtual Color Computer).

    VCC (Virtual Color Computer) is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VCC (Virtual Color Computer) is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VCC (Virtual Color Computer).  If not, see <http://www.gnu.org/licenses/>.
*/

/*--------------------------------------------------------------------------*/


#include <stdio.h>
#include <unistd.h>
#include "defines.h"
#include "fileops.h"
#include "resource.h"
#include "joystickinputSDL.h"
#include "vcc.h"
#include "tcc1014mmu.h"
#include "tcc1014graphicsAGAR.h"
#include "tcc1014registers.h"
#include "hd6309.h"
#include "mc6809.h"
#include "mc6821.h"
#include "keyboard.h"
#include "coco3.h"
#include "pakinterface.h"
#include "audio.h"
#include "config.h"
#include "quickload.h"
#include "throttle.h"
#include "logger.h"
#include "AGARInterface.h"

#include "xdebug.h"

SystemState2 EmuState2;
static bool DialogOpen=false;
static unsigned char Throttle=0;
static unsigned char AutoStart=1;
static unsigned char Qflag=0;
static char CpuName[20]="CPUNAME";
static char MmuName[20]="MMUNAME";
static int showLeftJoystickValues = 0;
static int showRightJoystickValues = 0;

char QuickLoadFile[256];
/***Forward declarations of functions included in this code module*****/

static void SoftReset(void);
void LoadIniFile(void);
void *EmuLoop(void *);
void (*CPUInit)(void)=NULL;
int  (*CPUExec)( int)=NULL;
void (*CPUReset)(void)=NULL;
void (*CPUAssertInterupt)(UINT8, UINT8)=NULL;
void (*CPUDeAssertInterupt)(UINT8)=NULL;
void (*CPUForcePC)(UINT16)=NULL;
PUINT8 (*MmuInit)(UINT8)=NULL;
void (*MmuReset)(void)=NULL;
void (*SetVectors)(UINT8)=NULL;
void (*SetMmuRegister)(UINT8, UINT8)=NULL;
void (*SetRomMap)(UINT8)=NULL;
void (*SetMapType)(UINT8)=NULL;
void (*Set_MmuTask)(UINT8)=NULL;
void (*Set_MmuEnabled)(UINT8)=NULL;
PUINT8 (*Getint_rom_pointer)(void)=NULL;
void (*CopyRom)(void)=NULL;
UINT8 (*MmuRead8)(UINT8, UINT16)=NULL;
void (*MmuWrite8)(UINT8, UINT8, UINT16)=NULL;
UINT8 (*MemRead8)(UINT16)=NULL;
void (*MemWrite8)(UINT8, UINT16)=NULL;
void (*SetDistoRamBank)(UINT8)=NULL;

// Message handlers

// Globals

#ifdef _DEBUG
# ifdef DARWIN
FILE *logg;
# endif
#endif

char *GlobalExecFolder;
char *GlobalFullName;
char *GlobalShortName;
void HandleSDLevent(SDL_Event);
void FullScreenToggleAGAR(void);
void InvalidateBoarderAGAR(void);

static char g_szAppName[MAX_LOADSTRING] = "";
bool BinaryRunning;
static unsigned char FlagEmuStop=TH_RUNNING;

void DecorateWindow(SystemState2 *);
void AddDummyCartMenus(void);
void RemoveDummyCartMenus(void);
void PrepareEventCallBacks(SystemState2 *);
void PadDummyCartMenus(void);

/*--------------------------------------------------------------------------*/


int main(int argc, char **argv)
{
	char cwd[260];
	char name[260];
	char execpath[260];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		GlobalExecFolder = cwd;
	} 
	else
	{
		GlobalExecFolder = argv[0];
		PathRemoveFileSpec(GlobalExecFolder);
	}

	GlobalFullName = argv[0];

	char temp1[MAX_PATH]="";
	char temp2[MAX_PATH]=" Running on ";
	AG_Thread threadID;

	// This Application Name
	strcpy(name, argv[0]);
	PathStripPath(name);
	GlobalShortName = name;

	// fprintf(stderr, "argv[0] : %s\n", GlobalFullName);
	// fprintf(stderr, "cwd     : %s\n", GlobalExecFolder);
	// fprintf(stderr, "name    : %s\n", GlobalShortName);

	strcpy(g_szAppName, GlobalShortName);

	if (argc > 1 && strlen(argv[1]) !=0)
	{
		strcpy(QuickLoadFile, argv[1]);
		strcpy(temp1, argv[1]);
		PathStripPath(temp1);
		SDL_strlwr(temp1);
		temp1[0]=toupper(temp1[0]);
		strcat (temp1, temp2);
		strcat(temp1, g_szAppName);
		strcpy(g_szAppName, temp1);
	}

	EmuState2.WindowSize.x=640;
	EmuState2.WindowSize.y=480;
	
	if (!CreateAGARWindow(&EmuState2))
	{
		fprintf(stderr,"Can't create AGAR Window\n");
	}

	fprintf(stderr, "GlobalExecFolder: %s\n", GlobalExecFolder);
	fprintf(stderr, "GlobalFullName: %s\n", GlobalFullName);
	fprintf(stderr, "GlobalShortName: %s\n", GlobalShortName);

	if (strcmp(GlobalExecFolder, "/") == 0)
	{
		char *str, *laststr = NULL;

		AG_Strlcpy(execpath, argv[0], sizeof(execpath));
		str = execpath;

		while ((str = strstr(str, "/Contents")) != NULL)
		{
			laststr = str;
			str += 9;
		}
		if (laststr != NULL)
		{
			laststr += 9;
			*laststr = 0;
		}

		GlobalExecFolder = execpath;
		chdir(GlobalExecFolder);
	}

#ifdef _DEBUG
# ifdef DARWIN
	{
		size_t savlen = strlen(GlobalExecFolder);

		strcat(GlobalExecFolder, GetPathDelimStr());
		strcat(GlobalExecFolder, "ovcc.log");
		fprintf(stderr, "Opening log file: %s\n", GlobalExecFolder);
		logg = fopen(GlobalExecFolder, "w");
		if (!logg)
			fprintf(stderr, "Couldn't open ovcc.log\n");
		else
			setbuf(logg, NULL);
		GlobalExecFolder[savlen] = 0;
	}
# endif
#endif

	XTRACE("GlobalExecFolder: %s\n", GlobalExecFolder);

	DecorateWindow(&EmuState2);

	AG_WindowShow(EmuState2.agwin);

    EmuState2.SurfacePitch = 640;

	PrepareEventCallBacks(&EmuState2);

	ClsAGAR(0, &EmuState2);

	LoadConfig(&EmuState2);			//Loads the default config file Vcc.ini from the exec directory
	PadDummyCartMenus();
	EmuState2.ResetPending=2;
	SetClockSpeed(1);	//Default clock speed .89 MHZ	
	BinaryRunning = true;
	EmuState2.EmulationRunning=AutoStart;

	if ((argc > 1 && strlen(argv[1]) != 0))
	{
		Qflag=255;
		EmuState2.EmulationRunning=1;
	}

	if (AG_ThreadTryCreate(&threadID, EmuLoop, NULL) != 0)
	{
		fprintf(stderr, "Can't Start main Emulation Thread!\n");
		return(0);
	}

	EmuState2.emuThread = threadID;

#ifdef ISOCPU
	extern void *CPUloop(void *);
	if (AG_ThreadTryCreate(&threadID, CPUloop, &EmuState2) != 0)
	{
		fprintf(stderr, "Can't Start CPU Thread!\n");
		return(0);
	}
#endif

	EmuState2.cpuThread = threadID;

    AG_EventLoop();
	
	EmuState2.Pixels = NULL;
	EmuState2.EmulationRunning = 0;

	//AG_ThreadCancel(threadID);

	//UnloadDll(0);
	//SoundDeInitSDL();
	//WriteIniFile(); //Save Any changes to ini FileS

#ifdef _DEBUG
# ifdef DARWIN
	fclose(logg);
# endif
#endif

	return 0;
}

/* Call backs for Events from GUI System */

void DoHardResetF9()
{
    extern void SetStatusBarText(const char *, SystemState2 *);

	//EmuState2.EmulationRunning=!EmuState2.EmulationRunning;
	if ( EmuState2.EmulationRunning )
		EmuState2.ResetPending=2;
	else
		SetStatusBarText("", &EmuState2);
}

void DoSoftReset()
{
	if ( EmuState2.EmulationRunning )
	{
		EmuState2.ResetPending = 1;
	}
}

void ToggleMonitorType()
{
	SetMonitorTypeAGAR(!SetMonitorTypeAGAR(QUERY));
}

void ToggleThrottleSpeed()
{
	SetSpeedThrottle(!SetSpeedThrottle(QUERY));
}

void ToggleScreenStatus()
{
	SetInfoBandAGAR(!SetInfoBandAGAR(QUERY));
    InvalidateBoarderAGAR();
}

void ToggleFullScreen()
{
	if (FlagEmuStop == TH_RUNNING)
	{
		FlagEmuStop = TH_REQWAIT;
		EmuState2.FullScreen = !EmuState2.FullScreen;
		FullScreenToggleAGAR();
		FlagEmuStop = TH_RUNNING;
	}
}

void DoMouseMotion(int ex, int ey)
{
	if (EmuState2.EmulationRunning)
	{
		int x = ex;
		int y = ey;

		joystickSDL(x,y);
		//fprintf(stderr, "Mouse @ %i - %i\n", x, y);
	}
}

void SetCPUMultiplyerFlag (unsigned char double_speed)
{
	unsigned short clockspeed = 1;
	EmuState2.DoubleSpeedFlag=double_speed;
	EmuState2.CPUCurrentSpeed= .894;
	if (EmuState2.DoubleSpeedFlag)
		clockspeed =  EmuState2.DoubleSpeedMultiplyer * EmuState2.TurboSpeedFlag;
	if (EmuState2.DoubleSpeedFlag)
		EmuState2.CPUCurrentSpeed *= (EmuState2.DoubleSpeedMultiplyer*EmuState2.TurboSpeedFlag);
	SetClockSpeed(clockspeed); 
	return;
}

void SetTurboMode(unsigned char data)
{
	unsigned short clockspeed = 1;
	EmuState2.TurboSpeedFlag=(data&1)+1;
	if (EmuState2.DoubleSpeedFlag)
		clockspeed = EmuState2.DoubleSpeedMultiplyer * EmuState2.TurboSpeedFlag;
	EmuState2.CPUCurrentSpeed= .894;
	if (EmuState2.DoubleSpeedFlag)
		EmuState2.CPUCurrentSpeed*=(EmuState2.DoubleSpeedMultiplyer*EmuState2.TurboSpeedFlag);
	SetClockSpeed(clockspeed); 
	return;
}

unsigned char SetCPUMultiplyer(unsigned short Multiplyer)
{
	if (Multiplyer!=QUERY)
	{
		EmuState2.DoubleSpeedMultiplyer=Multiplyer;
		SetCPUMultiplyerFlag (EmuState2.DoubleSpeedFlag);
	}
	return(EmuState2.DoubleSpeedMultiplyer);
}

void DoHardReset(SystemState2* const HRState)
{	
	//fprintf(stderr, "DoHardReset %d\n", HRState->MmuType);
	
	switch (HRState->MmuType)
	{	case 0: // Software MMU
			SetSWMmu();
			break;
#ifndef __MINGW32__
		case 1: // Hardware MMU
			SetHWMmu();
			break;
#endif
	}

	HRState->RamBuffer=MmuInit(HRState->RamSize);	//Alocate RAM/ROM & copy ROM Images from source
	//TriggerModuleShare(2); // 2 = reshare PAK Ext ROM
	HRState->WRamBuffer=(unsigned short *)HRState->RamBuffer;
	EmuState2.RamBuffer=HRState->RamBuffer;
	EmuState2.WRamBuffer=HRState->WRamBuffer;
	if (HRState->RamBuffer == NULL)
	{
		fprintf(stderr,"Can't allocate enough RAM, Out of memory\n");
		exit(0);
	}
	switch (HRState->CpuType)
	{
		case 0: // 6809
		CPUInit=MC6809Init;
		CPUExec=MC6809Exec;
		CPUReset=MC6809Reset;
		CPUAssertInterupt=MC6809AssertInterupt;
		CPUDeAssertInterupt=MC6809DeAssertInterupt;
		CPUForcePC=MC6809ForcePC;
		break;
		case 1: // 6309
		CPUInit=HD6309Init;
		switch (HRState->MouseType) // Mouse type determines which CPU exec we use
		{
			case 0: CPUExec=HD6309Exec; /* fprintf(stdout, "CPU Exec\n"); */ break;
			case 1: CPUExec=HD6309ExecHiRes; /* fprintf(stdout, "CPU Exec Hi-Res\n"); */ break;
			default: CPUExec=HD6309Exec; /* fprintf(stdout, "CPU Exec\n"); */ break;
		}
		CPUReset=HD6309Reset;
		CPUAssertInterupt=HD6309AssertInterupt;
		CPUDeAssertInterupt=HD6309DeAssertInterupt;
		CPUForcePC=HD6309ForcePC;
		break;
	}

	PiaReset();
	mc6883_reset();	//Captures interal rom pointer for CPU Interupt Vectors
	CPUInit();
	CPUReset();		// Zero all CPU Registers and sets the PC to VRESET
	GimeResetAGAR();
	UpdateBusPointer();
	EmuState2.DoubleSpeedFlag=0;
	EmuState2.TurboSpeedFlag=1;
	ResetBus();
	SetClockSpeed(1);
	return;
}

static void SoftReset(void)
{
	mc6883_reset(); 
	PiaReset();
	CPUReset();
	GimeResetAGAR();
	MmuReset();
	CopyRom();
	ResetBus();
	EmuState2.TurboSpeedFlag=1;
	return;
}

unsigned char SetRamSize(unsigned char Size)
{
	if (Size!=QUERY)
	{
		EmuState2.RamSize=Size;
		EmuState2.RamSize=Size;
	}
	return(EmuState2.RamSize);
}

unsigned char SetSpeedThrottle(unsigned char throttle)
{
	if (throttle!=QUERY)
	{
		CalibrateThrottle();
		Throttle=throttle;
	}
	return(Throttle);
}

unsigned char SetFrameSkip(unsigned char Skip)
{
	if (Skip!=QUERY)
		EmuState2.FrameSkip=Skip;
	return(EmuState2.FrameSkip);
}

unsigned char SetCpuType( unsigned char Tmp)
{
	switch (Tmp)
	{
	case 0:
		EmuState2.CpuType=0;
		strcpy(CpuName,"MC6809");
		break;

	case 1:
		EmuState2.CpuType=1;
		strcpy(CpuName,"HD6309");
		break;
	}
	return(EmuState2.CpuType);
}

unsigned char SetMmuType(unsigned char Tmp)
{
	switch (Tmp)
	{
	case 0:
		EmuState2.MmuType=0;
		strcpy(MmuName,"Software Simulation");
		break;

	case 1:
		EmuState2.MmuType=1;
		strcpy(MmuName,"Hardware Emulation");
		break;
	}
	return(EmuState2.MmuType);
}

void DoReboot(void)
{
	EmuState2.ResetPending=2;
	return;
}

unsigned char SetAutoStart(unsigned char Tmp)
{
	if (Tmp != QUERY)
		AutoStart=Tmp;
	return(AutoStart);
}

/* Here starts the main Emulation Loop*/

static char NatEmuStat[2] = "";

void SetNatEmuStat(unsigned char natemu)
{
	switch (natemu)
	{
		case 1: strcpy(NatEmuStat, "E"); break;
		case 2:	strcpy(NatEmuStat, "N"); break;
		default: strcpy(NatEmuStat, ""); break;
	}
}

static char MMUStat[2] = "";

void SetMMUStat(unsigned char mmu)
{
	switch (mmu)
	{
		case 0: strcpy(MMUStat, "S"); break;
		case 1:	strcpy(MMUStat, "H"); break;
		default: strcpy(MMUStat, ""); break;
	}
}

void *EmuLoop(void *p)
{
	static float FPS;
	static unsigned int FrameCounter=0;	
	CalibrateThrottle();
	AG_Delay(30);
	unsigned long LC = 0;
	int framecnt = 0;
	static char ttbuff[256];

	//TestDelay();

	//printf("Entering Emu Loop : Skip %i - Reset : %i\n", (int)EmuState2.FrameSkip, (int)EmuState2.ResetPending);

	while (1) 
	{
		if (FlagEmuStop==TH_REQWAIT)
		{
			//printf("delaying\n");
			FlagEmuStop=TH_WAITING;	//Signal Main thread we are waiting
			while(FlagEmuStop==TH_WAITING)
				AG_Delay(1);
			//printf("finished delaying\n");
		}

		FPS=0;
		if ((Qflag==255) & (FrameCounter==30))
		{
			Qflag=0;
			QuickLoad(QuickLoadFile);
		}

		StartRender();
		for (uint8_t Frames = 1; Frames <= EmuState2.FrameSkip; Frames++)
		{
			FrameCounter++;
			if (EmuState2.ResetPending != 0) {
				switch (EmuState2.ResetPending)
				{
				case 1:	//Soft Reset
					//printf("soft reset\n");
					SoftReset();
					break;

				case 2:	//Hard Reset
					//printf("hard reset\n");
					UpdateConfig();
					DoClsAGAR(&EmuState2);
					DoHardReset(&EmuState2);
					break;

				case 3:
					//printf("docls\n");
					DoClsAGAR(&EmuState2);
					break;

				case 4:
					//printf("upd conf\n");
					UpdateConfig();
					DoClsAGAR(&EmuState2);
					break;

				default:
					break;
				}
				EmuState2.ResetPending = 0;
			}

			if (EmuState2.EmulationRunning == 1) {
				FPS += RenderFrame(&EmuState2, LC);
			} else {
				FPS += StaticAGAR(&EmuState2);
			}
		}

		EndRender(EmuState2.FrameSkip);
		FPS = FPS != 0.0 ? FPS/EmuState2.FrameSkip : GetCurrentFPS()/EmuState2.FrameSkip;
		GetModuleStatus(&EmuState2);

		// Update status bar

		char tmpbuf[256];
		sprintf(ttbuff, "FPS:%3.0f|%s%s%s@%3.2fMhz", FPS,CpuName,NatEmuStat,MMUStat,EmuState2.CPUCurrentSpeed);

		if(showLeftJoystickValues)
		{
			extern void GetLeftJoystickValues(int*, int*);
			int joyx, joyy;
			strcpy(tmpbuf, ttbuff);
			GetLeftJoystickValues(&joyx, &joyy);
			sprintf(ttbuff,"%s|LX:%3d-LY:%3d", tmpbuf, joyx,joyy);
		}

		if(showRightJoystickValues)
		{
			extern void GetRightJoystickValues(int*, int*);
			int joyx, joyy;
			strcpy(tmpbuf, ttbuff);
			GetRightJoystickValues(&joyx, &joyy);
			sprintf(ttbuff,"%s|RX:%3d-RY:%3d", tmpbuf, joyx,joyy);
		}

		strcpy(tmpbuf, ttbuff);
		sprintf(ttbuff,"%s|%s", tmpbuf,EmuState2.StatusLine);
    	extern void SetStatusBarText(const char *, SystemState2 *);
		SetStatusBarText(ttbuff, &EmuState2);

#ifndef ISOCPU
		if (Throttle)	//Do nothing until the frame is over returning unused time to OS
		{
    		//fprintf(stderr, "4(%2.3f)", timems());
			FrameWait();
    		//fprintf(stderr, "5(%2.3f)-", timems());
		}
#endif
	} //Still Emulating
	return(p);
}

void FullScreenToggleAGAR(void)
{
	EmuState2.EmulationRunning = 0;
	PauseAudioSDL(true);

	if (EmuState2.FullScreen)
	{	
		AG_WindowMaximize(EmuState2.agwin);
		//AG_WindowUnmaximize(EmuState2.agwin);
	}
	else{
		AG_WindowMinimize(EmuState2.agwin);
		AG_WindowUnminimize(EmuState2.agwin);
	}

	EmuState2.Resizing = 0;
	EmuState2.EmulationRunning = 1;
	InvalidateBoarderAGAR();
	PauseAudioSDL(false);
	return;
}

void SetShowLeftJoystickValue(int showJoystickVal)
{
	showLeftJoystickValues = showJoystickVal;
}

void SetShowRightJoystickValue(int showJoystickVal)
{
	showRightJoystickValues = showJoystickVal;
}
