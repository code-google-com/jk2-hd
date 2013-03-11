#ifndef _JK2_MAIN_H
#define _JK2_MAIN_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "jk2_gamestructs.h"
#include "jk2_hooks.h"

__declspec(dllexport) void __cdecl trap_Error(char *error);

extern cvar_t *in_invertXLook;
extern cvar_t *in_invertYLook;

#endif