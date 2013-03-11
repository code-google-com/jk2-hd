#ifndef _JK2_HOOKS_H
#define _JK2_HOOKS_H

#include "jk2_main.h"

#define DLLIDA(address, module) ((address - 0x20000000) + ((DWORD)module))
#define DLLJK2(address, module) (address + (DWORD)module)

/*==============================================*/
/** 
 *  @brief stdcall function pointer declaration.
 *
 *  Declares the type for a STDCALL function pointer
 *  called N with arguments list A adn returning R.
 *
 */
/*==============================================*/

#define STDCALL_FUNCTION( R, N, A )  typedef R (__stdcall *N) A

/*==============================================*/
/**
 *  @brief fastcall function pointer declaration.
 *
 *  Declares the type for a FASTCALL function pointer
 *  called N with arguments list A adn returning R.
 *
 */
/*==============================================*/

#define FASTCALL_FUNCTION( R, N, A ) typedef R (__fastcall *N) A

/*==============================================*/
/**
 *  @brief cdecl function pointer declaration.
 *
 *  Declares the type for a CDECL function pointer
 *  called N with arguments list A adn returning R.
 *
 */
/*==============================================*/

#define CDECL_FUNCTION( R, N, A ) typedef R (__cdecl *N) A

/*==============================================*/
/**
 *  @brief thiscall function pointer declaration.
 *
 *  Declares the type for a THISCALL function pointer
 *  called N with arguments list A adn returning R.
 *
 */
/*==============================================*/

#define THISCALL_FUNCTION( R, N, A ) typedef R (__thiscall *N) A

/*==============================================*/
/*
 * blank function type
 */
/*==============================================*/

#define BLANK_FUNCTION( R, N, A ) typedef R (*N) A

extern HMODULE jagamex86;
extern HMODULE proxyDLL;

extern DWORD vmMainAddy;
extern DWORD dllEntryAddy;
extern DWORD GetGameAPIAddy;
extern DWORD jagamex86BaseAddy;

typedef struct {
	int addr;
	int size;
	char origbytes[24];
} PatchData_t;

typedef enum {
	PATCH_JUMP,
	PATCH_CALL,
} PatchType_e;

extern PatchData_t *JK2_PlacePatch( int type, unsigned int address, unsigned int destination );
extern void JK2_RemovePatch(PatchData_t **patch);
extern void JK2_BytePatching(unsigned char data, DWORD location);
extern void JK2_LongPatching(long data, DWORD location);

void DoBalanceChanges(void);
void DoFunctionLinkage(void);
void DoVisualHacks(void);
void RemoveVisualHacks(void);

#endif