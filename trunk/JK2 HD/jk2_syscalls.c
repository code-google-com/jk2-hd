#include "jk2_main.h"

extern int (__cdecl *syscall)( int arg, ... );

__declspec(dllexport) void __cdecl trap_Error(char *error)
{
	syscall(SYSCALL_ERROR, error);
}