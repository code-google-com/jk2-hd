#ifndef __JK2_GAMEFUNCS_H
#define __JK2_GAMEFUNCS_H

#include "jk2_hooks.h"

// HERE BE MACROS
#define BUMP(x,y)	if(x < y) x = y
#define  CAP(x,y)	if(x > y) x = y
#define CLAMP( var, min, max )			BUMP( var,		min );	CAP( var,		max)
#define CLAMPVEC( vec, idx, min, max )	BUMP( vec[idx], min );	CAP( vec[idx],	max)

extern signed char ClampChar( int i );
extern signed short ClampShort( int i );

// CGame
CDECL_FUNCTION(const char *, _CG_ConfigString, (int stringIndex));
CDECL_FUNCTION(char *, _Info_ValueForKey, (const char *string, const char *key));
CDECL_FUNCTION(qhandle_t, _cgi_R_RegisterShaderNoMip, (const char *shader));
CDECL_FUNCTION(void, _cgi_R_SetColor, (vec3_t color));
CDECL_FUNCTION(void, _CG_DrawPic, (float x, float y, float w, float h, qhandle_t shader));
CDECL_FUNCTION(void, _CG_DrawSavegamePic, (float x, float y, float w, float h));
CDECL_FUNCTION(void, _CG_DrawLevelStats, (void));
CDECL_FUNCTION(void, _CG_DrawLoadBar, (void));
CDECL_FUNCTION(char *, _va, (char *format, ...));
CDECL_FUNCTION(void, _CG_CenterPrint, (char *text, int height));

// Engine
CDECL_FUNCTION(cvar_t *, _Cvar_Get, (const char *name, const char *defaultValue, int flags));
CDECL_FUNCTION(void, _IN_JoyMove, (void));
CDECL_FUNCTION(void, _CL_JoystickMove, (usercmd_t *));
CDECL_FUNCTION(void, _Sys_QueEvent, (int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr));
CDECL_FUNCTION(void, _CBuf_ExecuteText, (int when, char *text));
CDECL_FUNCTION(void, _Com_Printf, (char *format, ...));
CDECL_FUNCTION(void, _Com_Error, (int level, char *format, ...));

void Q_strncpyz( char *dest, const char *src, int destsize );
int Q_stricmpn (const char *s1, const char *s2, int n);
int Q_strncmp (const char *s1, const char *s2, int n);
int Q_stricmp (const char *s1, const char *s2);

#endif