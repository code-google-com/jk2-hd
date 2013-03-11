#include "jk2_main.h"

int (__cdecl *syscall)( int arg, ... ) = (int (__cdecl *)( int, ...))-1;

CDECL_FUNCTION(int, vmMainCGAME, ( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7 ));
CDECL_FUNCTION(void, dllEntryCGAME, (int (__cdecl  *syscallPtr)( int arg,... )));
CDECL_FUNCTION(game_export_t *, GetGameAPIGAME, (game_import_t *import));

vmMainCGAME vmGameX;
dllEntryCGAME dllEntryX;
GetGameAPIGAME GetGameAPIX;

DWORD vmMainAddy;
DWORD dllEntryAddy;
DWORD GetGameAPIAddy;
DWORD jagamex86BaseAddy;

HMODULE jagamex86;
HMODULE proxyDLL;

game_import_t *gi;
game_export_t *ge;

__declspec(dllexport) game_export_t *GetGameAPI(game_import_t *import)
{
	gi = import;
	ge = GetGameAPIX(import);
	return ge;
}

static void __stdcall Attach(qboolean useGI)
{
	if(!useGI)
	{
		jagamex86 = LoadLibrary(TEXT("ProxyDLL.dll"));
		jagamex86BaseAddy = (DWORD)GetModuleHandle("ProxyDLL.dll");
	}

	if(!jagamex86)
	{
		//todo: Call Com_Error direct from JASP.exe
		//trap_Error("PROXY: Could not load module\n");
		return;
	}
	else
	{
		DoFunctionLinkage();
		DoBalanceChanges();
		DoVisualHacks();
		JOY_Hacks(qfalse);
	}

	if(!useGI)
	{
		//Basic/absolutely essential funcs
		
		vmGameX = (vmMainCGAME)(GetProcAddress(jagamex86, "vmMain"));
		vmMainAddy = (DWORD)(GetProcAddress(jagamex86, "vmMain"));
		dllEntryX = (dllEntryCGAME)(GetProcAddress(jagamex86, "dllEntry"));
		dllEntryAddy = (DWORD)(GetProcAddress(jagamex86, "dllEntry"));
		GetGameAPIX = (GetGameAPIGAME)(GetProcAddress(jagamex86, "GetGameAPI"));
		GetGameAPIAddy = (DWORD)(GetProcAddress(jagamex86, "GetGameAPI"));
		//Extra/fun funcs
	}
}

static void __stdcall Detach(void)
{
	RemoveVisualHacks();
	JOY_Hacks(qtrue);
	FreeLibrary(jagamex86);		//maybe not necessary
	FreeLibrary(proxyDLL);
}

int __stdcall DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
			Attach(qfalse);
			break;
		case DLL_PROCESS_DETACH:
			Detach();
			break;
	}
	return 1;
}

__declspec (dllexport) int __stdcall DllEntryPoint(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
			Attach(qfalse);
			break;
		case DLL_PROCESS_DETACH:
			Detach();
			break;
	}
	return 1;
}

__declspec(dllexport) void dllEntry( int (__cdecl  *syscallPtr)( int arg,... ) ) {
	syscall = syscallPtr;
	//Unknown code here..just going to call SP's dllEntry for now since unknown
	dllEntryX(syscallPtr);
	gi->Printf("^2ProxyDLL: dllEntry OK\n");
	Attach(qtrue);
}

__declspec(dllexport) int vmMain(int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7)
{
	return vmGameX(command, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}