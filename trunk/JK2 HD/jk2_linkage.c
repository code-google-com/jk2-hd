#include "jk2_hooks.h"
#include "jk2_gamefuncs.h"

_CG_ConfigString CG_ConfigString;
_Info_ValueForKey Info_ValueForKey;
_cgi_R_RegisterShaderNoMip cgi_R_RegisterShaderNoMip;
_cgi_R_SetColor cgi_R_SetColor;
_CG_DrawPic CG_DrawPic;
_CG_DrawSavegamePic CG_DrawSavegamePic;
_CG_DrawLevelStats CG_DrawLevelStats;
_CG_DrawLoadBar CG_DrawLoadBar;
_va va;
_CG_CenterPrint CG_CenterPrint;

_Cvar_Get Cvar_Get;
_IN_JoyMove IN_JoyMove;
_Sys_QueEvent Sys_QueEvent;
_CL_JoystickMove CL_JoystickMove;
_CBuf_ExecuteText CBuf_ExecuteText;
_Com_Printf Com_Printf;
_Com_Error Com_Error;

void DoFunctionLinkage(void)
{
	// Not exactly pretty, but it gets the job done, amirite?
	CG_ConfigString = (_CG_ConfigString)DLLJK2(0x33B70, jagamex86);
	Info_ValueForKey = (_Info_ValueForKey)DLLJK2(0xE8D60, jagamex86);
	cgi_R_RegisterShaderNoMip = (_cgi_R_RegisterShaderNoMip)DLLJK2(0x464D0, jagamex86);
	cgi_R_SetColor = (_cgi_R_SetColor)DLLJK2(0x466C0, jagamex86);
	CG_DrawPic = (_CG_DrawPic)DLLJK2(0x29A80, jagamex86);
	CG_DrawSavegamePic = (_CG_DrawSavegamePic)DLLJK2(0x46760, jagamex86);
	CG_DrawLevelStats = (_CG_DrawLevelStats)DLLJK2(0x44560, jagamex86);
	CG_DrawLoadBar = (_CG_DrawLoadBar)DLLJK2(0x31210, jagamex86);
	va = (_va)DLLJK2(0xE8D20, jagamex86);
	CG_CenterPrint = (_CG_CenterPrint)DLLJK2(0x47820, jagamex86);

	Cvar_Get = (_Cvar_Get)0x4168C0;
	IN_JoyMove = (_IN_JoyMove)0x440380;
	Sys_QueEvent = (_Sys_QueEvent)0x441180;
	CL_JoystickMove = (_CL_JoystickMove)0x406A80;
	CBuf_ExecuteText = (_CBuf_ExecuteText)0x413E30;
	Com_Printf = (_Com_Printf)0x414730;
	Com_Error = (_Com_Error)0x414910;
}