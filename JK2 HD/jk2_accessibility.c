// Adding in some sweet XBOX 360 controller support. Cool stuff eh?
// The game follows the same controller setup as the console version of the game, with a few differences.
#include "jk2_main.h"
#include "jk2_hooks.h"
#include "jk2_gamefuncs.h"
#include <windows.h>
#include <XInput.h>
#include <math.h>
#pragma comment(lib, "XInput.lib")
#include <MMSystem.h>

extern _Cvar_Get Cvar_Get;
extern _IN_JoyMove IN_JoyMove;
extern _Sys_QueEvent Sys_QueEvent;
extern _CG_CenterPrint CG_CenterPrint;
extern _CL_JoystickMove CL_JoystickMove;
extern _CBuf_ExecuteText CBuf_ExecuteText;
extern _Com_Printf Com_Printf;

cvar_t *in_invertXLook = NULL;
cvar_t *in_invertYLook = NULL;

/*
===========
JoyToF
===========
*/
float JoyToF( int value ) {
	float	fValue;

	// move centerpoint to zero
	value -= 32768;

	// convert range from -32768..32767 to -1..1 
	fValue = (float)value / 32768.0;

	if ( fValue < -1 ) {
		fValue = -1;
	}
	if ( fValue > 1 ) {
		fValue = 1;
	}
	return fValue;
}

int JoyToI( int value ) {
	// move centerpoint to zero
	value -= 32768;

	return value;
}

// CONTROL SCHEME
// DPAD UP = toggle next force power. Go through UI elements (when in UI)
// DPAD LEFT/RIGHT - toggle next weapon (when ingame). Go through UI elements (when in UI)
// DPAD DOWN - toggle next inventory item, go through UI elements (when in UI)
// Left Thumbstick - Move
// Left Bumper - Force Power
// Right Thumbstick - view
// Right Bumper - Inventory item
// Left Trigger - Alt Fire
// Right Trigger - Primary Fire
// A - Jump/Press UI element
// X - Use
// B - Toggle Crouch
// Y - Saber style
// Select - Datapad
// Start - Menu

// States
static XINPUT_STATE state;
static XINPUT_VIBRATION XBOX_rumble;
static int lastJoy;

static qboolean toggleCrouch = qfalse;

// Rumble
static int rumbleTime;
static int rumbleStartTime;
static int rumbleIntensity;

DWORD joyTime;

// Debouncing
DWORD buttonYDebounce;
DWORD buttonBDebounce;
DWORD rightBumperDebounce;
DWORD dPadDebounce;
DWORD backButtonDebounce;
DWORD startButtonDebounce;
DWORD shoulderDebounce;

qboolean holdingForce = qfalse;

float *ptrPitch = (float *)0x822EF0;
float *ptrYaw = (float *)0x822EF4;

void XboxInit(void)
{
	// Called whenever the latched cvar in_joystick changes on the cgame end.
	// Controllers of all kinds cannot be used for UI, period.
	buttonYDebounce = buttonBDebounce = rightBumperDebounce = shoulderDebounce = dPadDebounce = 0;

	ZeroMemory( &XBOX_rumble, sizeof( XINPUT_VIBRATION ) );
}

void XboxUpdate(void)
{
	// This acts as a portal for traffic to flow through.
	// Vibration goes into the XBOX controller through this function, and we send commands to the game
	// from input received in the controller itself.
	DWORD dwResult;

	ZeroMemory( &state, sizeof(XINPUT_STATE) );
	dwResult = XInputGetState(0, &state); //Get the XInput state

	if(dwResult != ERROR_SUCCESS)
	{
		// Bye.
		CG_CenterPrint( "Please connect your XBOX 360 controller to your USB drive.", (int)(SCREEN_HEIGHT * 0.30));
		return;
	}

	// Left Sticks control movement
	Sys_QueEvent(*((int *)0x12E874C), SE_JOYSTICK_AXIS, AXIS_SIDE, state.Gamepad.sThumbLX, 0, NULL);
	Sys_QueEvent(*((int *)0x12E874C), SE_JOYSTICK_AXIS, AXIS_FORWARD, state.Gamepad.sThumbLY, 0, NULL);

	// Right Sticks control angles
	Sys_QueEvent(*((int *)0x12E874C), SE_JOYSTICK_AXIS, AXIS_YAW, state.Gamepad.sThumbRX, 0, NULL);
	Sys_QueEvent(*((int *)0x12E874C), SE_JOYSTICK_AXIS, AXIS_PITCH, state.Gamepad.sThumbRY, 0, NULL);

	// TODO: rumble
}

void XboxMovement(usercmd_t *cmd)
{
	int *joystickAxis = (int *)0x82274C;
	float rightratio = joystickAxis[AXIS_SIDE] / 32767.0f;
	float fwdratio = joystickAxis[AXIS_FORWARD] / 32767.0f;
	float pitchratio = joystickAxis[AXIS_PITCH] / 32767.0f;
	float yawratio = joystickAxis[AXIS_YAW] / 32767.0f;
	cvar_t *cl_yawspeed = Cvar_Get("cl_yawspeed", "140", 0);
	cvar_t *cl_pitchspeed = Cvar_Get("cl_pitchspeed", "140", 0);
	cvar_t *in_joyBallScale = Cvar_Get("in_joyBallScale", "0.02", 0);
	cvar_t *joy_threshold = Cvar_Get("joy_threshold", "0.15", 0);
	qboolean invertX;
	qboolean invertY;

	if(!in_invertXLook)
	{
		in_invertXLook = Cvar_Get("in_invertXLook", "0", 0x01);
	}
	if(!in_invertYLook)
	{
		in_invertYLook = Cvar_Get("in_invertYLook", "0", 0x01);
	}

	invertX = (qboolean)in_invertXLook->integer;
	invertY = (qboolean)in_invertYLook->integer;

	if(invertX)
	{
		yawratio *= -1.0f;
	}
	if(invertY)
	{
		pitchratio *= -1.0f;
	}

	if( state.Gamepad.wButtons & XINPUT_GAMEPAD_X )
	{
		cmd->buttons |= BUTTON_USE;
	}
	else if( state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER && *(DWORD *)DLLJK2(0x22B910, jagamex86) != 1)
	{
		// The shoulder buttons are dedicated lean buttons
		cmd->rightmove = -128;
		cmd->buttons |= BUTTON_USE;
	}
	else if( state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER && *(DWORD *)DLLJK2(0x22B910, jagamex86) != 1)
	{
		// The shoulder buttons are dedicated lean buttons
		cmd->rightmove = 127;
		cmd->buttons |= BUTTON_USE;
	}
	else
	{
		if(fabs(rightratio) > 0.3)
		{
			// WIP deadzone
			cmd->rightmove = ClampChar((int)(rightratio * 128));
		}

		if( fabs(fwdratio) > 0.3 )
		{
			cmd->forwardmove = ClampChar((int)(fwdratio * 128));
		}
	}

	// so going down is not faster than going up, and same with l/r
	if( fabs(pitchratio) > joy_threshold->value )
	{
		if(pitchratio < 0)
		{
			pitchratio += 0.1f;
			pitchratio *= -pitchratio;
		}
		else
		{
			pitchratio -= 0.1f;
			pitchratio *= pitchratio;
		}
		CLAMP(pitchratio, -1.0f, 1.0f);

		*ptrPitch -= (cl_pitchspeed->value*pitchratio)*in_joyBallScale->value;
	}

	if( fabs(yawratio) > joy_threshold->value )
	{
		if(yawratio < 0)
		{
			yawratio += 0.1f;
			yawratio *= -yawratio;
		}
		else
		{
			yawratio -= 0.1f;
			yawratio *= yawratio;
		}
		CLAMP(yawratio, -1.0f, 1.0f);

		*ptrYaw -= (cl_yawspeed->value*yawratio)*in_joyBallScale->value;
	}

	// Now, utilize the control scheme

	if( state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER && *(DWORD *)DLLJK2(0x22B910, jagamex86) == 1 && shoulderDebounce < joyTime )
	{
		// using a saber, and we press the left shoulder = cycle through force powers
		CBuf_ExecuteText(0, "forceprev");
		shoulderDebounce = joyTime + 200;
	}
	else if ( state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER && *(DWORD *)DLLJK2(0x22B910, jagamex86) == 1 && shoulderDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "forcenext");
		shoulderDebounce = joyTime + 200;
	}

	if( state.Gamepad.wButtons & XINPUT_GAMEPAD_A )
	{
		// A button = Jump
		cmd->upmove += 127;
	}

	if( state.Gamepad.bRightTrigger )
	{
		cmd->buttons |= BUTTON_ATTACK;
	}

	if( state.Gamepad.bLeftTrigger )
	{
		cmd->buttons |= BUTTON_ALT_ATTACK;
	}

	if( state.Gamepad.wButtons & XINPUT_GAMEPAD_Y && buttonYDebounce < joyTime && *(DWORD *)DLLJK2(0x22B910, jagamex86) == 1)
	{
		// Using a saber
		CBuf_ExecuteText(0, "saberAttackCycle");
		buttonYDebounce = joyTime + 350;
	}
	else if( state.Gamepad.wButtons & XINPUT_GAMEPAD_Y && buttonYDebounce < joyTime )
	{
		// Not using a saber? Switch to it.
		CBuf_ExecuteText(0, "weapon 1");
		buttonYDebounce = joyTime + 350;
	}

	if( state.Gamepad.wButtons & XINPUT_GAMEPAD_B && buttonBDebounce < joyTime)
	{
		toggleCrouch = (qboolean)!toggleCrouch;
		buttonBDebounce = joyTime + 350;
	}

	if( state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB )
	{
		CBuf_ExecuteText(0, "+useforce");
		holdingForce = qtrue;
	}
	else if(holdingForce)
	{
		CBuf_ExecuteText(0, "-useforce");
		holdingForce = qfalse;
	}

	if ( state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB && rightBumperDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "invuse");
		rightBumperDebounce = joyTime + 1000;
	}

	if( state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP && dPadDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "forcenext");
		dPadDebounce = joyTime + 300;
	}
	else if( state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN && dPadDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "invnext");
		dPadDebounce = joyTime + 300;
	}
	else if( state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT && dPadDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "weapprev");
		dPadDebounce = joyTime + 300;
	}
	else if( state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT && dPadDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "weapnext");
		dPadDebounce = joyTime + 300;
	}

	// Some new stuff I added recently --eez
	if( state.Gamepad.wButtons & XINPUT_GAMEPAD_START && startButtonDebounce < joyTime)
	{
		CBuf_ExecuteText(2, "save *quik");		// this has to be exec_append otherwise the save file isn't able to be written to!
		startButtonDebounce = joyTime + 2000;
	}

	if( state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK && backButtonDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "datapad");
		backButtonDebounce = joyTime + 1000;
	}

	if(toggleCrouch)
	{
		cmd->upmove = -127;
	}
}

DWORD PS3buttons = 0;
DWORD PS3POVhat = 0;

void PS3Init(void)
{
	// This is already handled by the game when it inits
}

void PS3Update(void)
{
	// This isn't filled out properly by id's stuff. 
	// It would appear that the movement controls in the sixaxis stuff is clouding this >_>'
	// so DualShock 3 might have different mapping from SixaxiS. I haven't figured that out yet.
	// Note to self: send message to minilogoguy about doing a test on in_debugjoystick
	JOYINFOEX pJi;
	cvar_t *in_debugJoystick = Cvar_Get("in_debugJoystick", "0", 0);

	if(!(*(DWORD *)0x12D9D98))
	{	// joy.avail
		return;
	}

	pJi = *(JOYINFOEX *)0x012D9F3C;
	memset(&pJi, 0, sizeof(JOYINFOEX));
	pJi.dwSize = sizeof(JOYINFOEX);
	// The Sixaxis controller has to be custom-calibrated. Fail.
	pJi.dwFlags = JOY_RETURNALL;

	if(joyGetPosEx(*(UINT *)0x012D9D9C, &pJi) != JOYERR_NOERROR)
	{
		return;
	}

	// U pos = down/up (they're flipped, marons)
	// V pos = left/right

	if(in_debugJoystick->integer)
	{
		float VPosFloat = (float)pJi.dwVpos;
		Com_Printf("%8x %5i %5.2f %5.2f %5.2f %5.2f %6i %6i\n",
			pJi.dwButtons,
			pJi.dwPOV,
			JoyToF( pJi.dwXpos ), JoyToF( pJi.dwYpos ),
			JoyToF( pJi.dwZpos ), JoyToF( pJi.dwRpos ),
			JoyToI( pJi.dwUpos ), JoyToI( pJi.dwVpos ));
	}

	PS3buttons = pJi.dwButtons;

	// POV hat is also inverted (marons++)
	PS3POVhat = pJi.dwPOV;

	// Alright, now handle the axes CORRECTLY (not like a maron)
	Sys_QueEvent(*((int *)0x12E874C), SE_JOYSTICK_AXIS, AXIS_SIDE, pJi.dwXpos, 0, NULL);
	Sys_QueEvent(*((int *)0x12E874C), SE_JOYSTICK_AXIS, AXIS_FORWARD, -pJi.dwYpos, 0, NULL);
	Sys_QueEvent(*((int *)0x12E874C), SE_JOYSTICK_AXIS, AXIS_UP, pJi.dwZpos, 0, NULL);			// L2/R2 trigger grouping
	Sys_QueEvent(*((int *)0x12E874C), SE_JOYSTICK_AXIS, AXIS_PITCH, -pJi.dwUpos, 0, NULL);
	Sys_QueEvent(*((int *)0x12E874C), SE_JOYSTICK_AXIS, AXIS_YAW, pJi.dwVpos, 0, NULL);
}

void PS3Movement(usercmd_t *cmd)
{
	int *joystickAxis = (int *)0x82274C;
	// I think these are already divided up? Not sure.
	float rightratio = (float)(joystickAxis[AXIS_SIDE]-32511)/32511.0f;
	float fwdratio = (float)(joystickAxis[AXIS_FORWARD]+32511)/32511.0f;
	float pitchratio = (float)(joystickAxis[AXIS_PITCH]+32511)/32511.0f;
	float yawratio = (float)(joystickAxis[AXIS_YAW]-32511)/32511.0f;
	float triggers = (float)(joystickAxis[AXIS_UP]-32767)/32767.0f;			// I mapped this to UP because that axis should never be used, hopefully.
																			// Even if it is used, I have the excuse that this works for ship combat.
	cvar_t *cl_yawspeed = Cvar_Get("cl_yawspeed", "140", 0);
	cvar_t *cl_pitchspeed = Cvar_Get("cl_pitchspeed", "140", 0);
	cvar_t *in_joyBallScale = Cvar_Get("in_joyBallScale", "0.02", 0);
	cvar_t *joy_threshold = Cvar_Get("joy_threshold", "0.15", 0);
	qboolean invertX;
	qboolean invertY;

	CLAMP(rightratio, -1.0f, 1.0f);
	CLAMP(fwdratio, -1.0f, 1.0f);
	CLAMP(pitchratio, -1.0f, 1.0f);
	CLAMP(yawratio, -1.0f, 1.0f);
	CLAMP(triggers, -1.0f, 1.0f);


	if(!in_invertXLook)
	{
		in_invertXLook = Cvar_Get("in_invertXLook", "0", 0x01);
	}
	if(!in_invertYLook)
	{
		in_invertYLook = Cvar_Get("in_invertYLook", "0", 0x01);
	}

	invertX = (qboolean)in_invertXLook->integer;
	invertY = (qboolean)in_invertYLook->integer;

	if(invertX)
	{
		yawratio *= -1;
	}
	if(invertY)
	{
		pitchratio *= -1;
	}

	if(PS3buttons & ( 1 << PS3_SQUARE ) )
	{
		// Dedicated use button
		cmd->buttons |= BUTTON_USE;
	}
	else if( PS3buttons & ( 1 << PS3_L1 ) && *(DWORD *)DLLJK2(0x22B910, jagamex86) != 1)
	{
		// The shoulder buttons are dedicated lean buttons
		cmd->rightmove = -128;
		cmd->buttons |= BUTTON_USE;
	}
	else if( PS3buttons & ( 1 << PS3_R1 ) && *(DWORD *)DLLJK2(0x22B910, jagamex86) != 1)
	{
		// The shoulder buttons are dedicated lean buttons
		cmd->rightmove = 127;
		cmd->buttons |= BUTTON_USE;
	}
	else
	{
		if(fabs(rightratio) > 0.3)
		{
			// WIP deadzone
			cmd->rightmove = ClampChar((int)(rightratio * 128));
		}

		if( fabs(fwdratio) > 0.3 )
		{
			cmd->forwardmove = ClampChar((int)(fwdratio * 128));
		}
	}

	// so going down is not faster than going up, and same with l/r
	if( fabs(pitchratio) > joy_threshold->value )
	{
		if(pitchratio < 0)
		{
			pitchratio += 0.1f;
			pitchratio *= -pitchratio;
		}
		else
		{
			pitchratio -= 0.1f;
			pitchratio *= pitchratio;
		}
		CLAMP(pitchratio, -1.0f, 1.0f);

		*ptrPitch -= (cl_pitchspeed->value*pitchratio)*in_joyBallScale->value;
	}

	if( fabs(yawratio) > joy_threshold->value )
	{
		if(yawratio < 0)
		{
			yawratio += 0.1f;
			yawratio *= -yawratio;
		}
		else
		{
			yawratio -= 0.1f;
			yawratio *= yawratio;
		}
		CLAMP(yawratio, -1.0f, 1.0f);

		*ptrYaw -= (cl_yawspeed->value*yawratio)*in_joyBallScale->value;
	}

	// Now, utilize the control scheme

	if( PS3buttons & ( 1 << PS3_L1 ) && *(DWORD *)DLLJK2(0x22B910, jagamex86) == 1 && shoulderDebounce < joyTime )
	{
		// using a saber, and we press the left shoulder = cycle through force powers
		CBuf_ExecuteText(0, "forceprev");
		shoulderDebounce = joyTime + 200;
	}
	else if ( PS3buttons & ( 1 << PS3_R1 ) && *(DWORD *)DLLJK2(0x22B910, jagamex86) == 1 && shoulderDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "forcenext");
		shoulderDebounce = joyTime + 200;
	}

	if( PS3buttons & ( 1 << PS3_CROSS ) )
	{
		// X = Jump
		cmd->upmove += 127;
	}

	if( triggers < 0.0f )
	{
		cmd->buttons |= BUTTON_ATTACK;
	}

	else if( triggers > 0.0f )
	{
		cmd->buttons |= BUTTON_ALT_ATTACK;
	}

	if( PS3buttons & ( 1 << PS3_TRIANGLE ) && buttonYDebounce < joyTime && *(DWORD *)DLLJK2(0x22B910, jagamex86) == 1)
	{
		// Using a saber
		CBuf_ExecuteText(0, "saberAttackCycle");
		buttonYDebounce = joyTime + 350;
	}
	else if( PS3buttons & ( 1 << PS3_TRIANGLE ) && buttonYDebounce < joyTime )
	{
		// Not using a saber? Switch to it.
		CBuf_ExecuteText(0, "weapon 1");
		buttonYDebounce = joyTime + 350;
	}

	if( PS3buttons & ( 1 << PS3_CIRCLE ) && buttonBDebounce < joyTime)
	{
		toggleCrouch = (qboolean)!toggleCrouch;
		buttonBDebounce = joyTime + 350;
	}

	if( PS3buttons & ( 1 << PS3_L3 ) )
	{
		CBuf_ExecuteText(0, "+useforce");
		holdingForce = qtrue;
	}
	else if(holdingForce)
	{
		CBuf_ExecuteText(0, "-useforce");
		holdingForce = qfalse;
	}

	if ( PS3buttons & ( 1 << PS3_R3 ) && rightBumperDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "invuse");
		rightBumperDebounce = joyTime + 1000;
	}

	// The D-Pad uses a POV hat, which isn't quite the same as the xbox interface.. --eez


	if( PS3POVhat == 0 && dPadDebounce < joyTime )		// hardcoded numbers are fun numbers --eez
	{
		CBuf_ExecuteText(0, "forcenext");
		dPadDebounce = joyTime + 300;
	}
	else if( PS3POVhat == 18000 && dPadDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "invnext");
		dPadDebounce = joyTime + 300;
	}
	else if( PS3POVhat == 27000 && dPadDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "weapprev");
		dPadDebounce = joyTime + 300;
	}
	else if( PS3POVhat == 9000 && dPadDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "weapnext");
		dPadDebounce = joyTime + 300;
	}
	else
	{
	}

	// Some new stuff I added recently --eez
	if( PS3buttons & ( 1 << PS3_START ) && startButtonDebounce < joyTime)		// SUPER DUPER MEGA HACK HERE
	{
		CBuf_ExecuteText(0, "bind JOY9 \"save *quik\"");
		startButtonDebounce = joyTime + 2000;
	}

	if( PS3buttons & ( 1 << PS3_SELECT ) && backButtonDebounce < joyTime )
	{
		CBuf_ExecuteText(0, "datapad");
		backButtonDebounce = joyTime + 1000;
	}

	if(toggleCrouch)
	{
		cmd->upmove = -127;
	}
}

void __cdecl ControllerUpdate(void)
{
	cvar_t *in_joystick = Cvar_Get("in_joystick", "0", 0x21);

	// First and foremost, let's check the new cvars out and make sure they're working
	if(!in_invertXLook)
	{
		in_invertXLook = Cvar_Get("in_invertXLook", "0", 0x01);
	}
	if(!in_invertYLook)
	{
		in_invertYLook = Cvar_Get("in_invertYLook", "0", 0x01);
	}

	joyTime = *(DWORD *)DLLJK2(0x22B51C, jagamex86);

	if(lastJoy != in_joystick->integer)
	{
		// Do the init function
		lastJoy = in_joystick->integer;
		if(lastJoy == 2)
		{
			// Xbox 360
			XboxInit();
		}
		else if(lastJoy == 3)
		{
			// Playstation 3
			PS3Init();
		}
	}
	else if(lastJoy == 0 || lastJoy == 1)
	{
		// Do the standard JK2 function
		IN_JoyMove();
		return;
	}
	else
	{
		if(lastJoy == 2)
		{
			XboxUpdate();
		}
		else if(lastJoy == 3)
		{
			// Playstation 3
			PS3Update();
		}
	}
}

void __cdecl ControllerMovement(usercmd_t *cmd)
{
	cvar_t *in_joystick = Cvar_Get("in_joystick", "0", 0x21);
	if(in_joystick->integer == 1 || in_joystick->integer == 0)
	{
		// Use id Tech's original way of doing it.
		CL_JoystickMove(cmd);
	}
	else if(in_joystick->integer == 2)
	{
		// XBOX 360
		XboxMovement(cmd);
	}
	else if(in_joystick->integer == 3)
	{
		// Playstation 3
		PS3Movement(cmd);
	}
}

PatchData_t *joystickHacks[2];

extern int UnlockMemory(int address, int size);
extern int LockMemory(int address, int size);
void JOY_Hacks(qboolean remove)
{
	lastJoy = 0;
	if(remove)
	{
		// Remove the patches
		JK2_RemovePatch(&joystickHacks[0]);
		JK2_RemovePatch(&joystickHacks[1]);
		// Lock the memory back up, there's no point in changing it at this point
		LockMemory((int)ptrPitch, sizeof(float));
		LockMemory((int)ptrPitch, sizeof(float));
	}
	else
	{
		// Place the patches, not remove them
		// 0x00440120 = IN_JoyMove call / first hook
		// 0x00406EFD = second hook. This is slightly different because it's a cdecl function, not a usercall one!
		joystickHacks[0] = JK2_PlacePatch(PATCH_CALL, 0x00440120, (unsigned int)ControllerUpdate);
		joystickHacks[1] = JK2_PlacePatch(PATCH_CALL, 0x00406EFD, (unsigned int)ControllerMovement);
		// Unlock the memory so that we can use it later
		UnlockMemory((int)ptrPitch, sizeof(float));
		UnlockMemory((int)ptrYaw, sizeof(float));
	}
}