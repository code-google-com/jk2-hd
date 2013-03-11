#include <stdlib.h>
#include <assert.h>
#include "jk2_main.h"
#include <libudis86/udis86.h>

// ==================================================
// UnlockMemory (WIN32 & Linux compatible)
// --------------------------------------------------
// Makes the memory at address writable for at least
// size bytes.
// Returns 1 if successful, returns 0 on failure.
// ==================================================
int UnlockMemory(int address, int size) {
	int ret;
	int dummy;
	ret = VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &dummy);
	return (ret != 0);
}

// ==================================================
// LockMemory (WIN32 & Linux compatible)
// --------------------------------------------------
// Makes the memory at address read-only for at least
// size bytes.
// Returns 1 if successful, returns 0 on failure.
// ==================================================
int LockMemory(int address, int size) {
	int ret;
	ret = VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READ, NULL);
	return (ret != 0);
}


// ==================================================
// JK2_PlacePatch (WIN32 & Linux compatible)
// By BobaFett
// --------------------------------------------------
// Patches the code at address to make a go towards
// destination.
// The instruction used is either JMP or CALL, 
// depending on the type specified.
//
// Before the code is modified, the code page is
// unlocked.
//
// This function returns a malloced PatchData_t.
// To remove the patch, call JKG_RemovePatch. This
// will also free the PatchData_t.
// ==================================================

PatchData_t *JK2_PlacePatch( int type, unsigned int address, unsigned int destination ) {
	PatchData_t *patch = malloc(sizeof(PatchData_t));
	//int addr = address;
	int sz = 0;

	ud_t ud;

	ud_init(&ud);
	ud_set_input_buffer(&ud, (uint8_t *)address, 48);
	ud_set_mode(&ud, 32);
	ud_set_pc(&ud, address);
	ud_set_syntax(&ud, NULL);

	while (ud_disassemble(&ud))
	{
		sz += ud_insn_len(&ud);
		if (sz >= 5) {
			break;
		}
	}

	assert(sz >= 5);

	if (sz < 5 || sz > 24) {
		// This really shouldnt ever happen, in the worst case scenario,
		// the block is 20 bytes (4 + 16), so if we hit 24, something went wrong
		return NULL;
	}

	patch->addr = address;
	patch->size = sz;
	memcpy(patch->origbytes, (const void *)address, sz);
	UnlockMemory(address, sz); // Make the memory writable
	*(unsigned char *)address = type == PATCH_JUMP ? 0xE9 : 0xE8;
	*(unsigned int *)(address+1) = destination - (address + 5);
	memset((void *)(address+5),0x90,sz-5);	// Nop the rest
	LockMemory(address, sz);
	return patch;
}

// ==================================================
// JK2_RemovePatch
// By BobaFett
// --------------------------------------------------
// Removes a previous code patch
// ==================================================

void JK2_RemovePatch(PatchData_t **patch) {
	if (!*patch)
		return;
	UnlockMemory((*patch)->addr, (*patch)->size);
	memcpy((void *)(*patch)->addr, (*patch)->origbytes, (*patch)->size);
	LockMemory((*patch)->addr, (*patch)->size);
	*patch = 0;
}

// Do byte patching
void JK2_BytePatching(unsigned char data, DWORD location)
{
	UnlockMemory(location, 1);
	*((unsigned char*)location) = data;
	LockMemory(location, 1);
}

// Do long patching
void JK2_LongPatching(long data, DWORD location)
{
	UnlockMemory(location, 4);
	*((long*)location) = data;
	LockMemory(location, 4);
}