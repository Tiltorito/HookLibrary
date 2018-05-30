// HookLibrary.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>

using namespace std;

typedef struct {
	DWORD dwSize; // number of data that overriden in bytes
	PBYTE data; // A pointer to an array of BYTES with size of dwData containing the data that have been overriden
	LPVOID lpAddress; // The address to jump back
} HOOK_INFO, *PHOOK_INFO;

HOOK_INFO hook_info = { 0 };



// simply trampoline
VOID SetupHookBefore(LPVOID func, LPVOID hook, SIZE_T size) {

}

// Change the ret address
VOID SetupHookAfter(LPVOID func, LPVOID hook, SIZE_T size) {

}

// Even tho it returns SIZE_T it can return negative offset it will just be seen as unsigned if you access it via SIZE_T
SIZE_T GetRelativeAddress(LPVOID from, LPVOID to) {
	return (SIZE_T)to - (SIZE_T)to - 5; // -5 for the jmp instruction size
}

SIZE_T GetAbsoluteAddress(LPVOID from, SIZE_T relativeAddress) {
	return 0;
}

/* Returns a Byte array containg the bytes that are overriden */
PBYTE FillWithNops(LPVOID lpAddress, DWORD dwSize) {
	PBYTE buffer = (PBYTE)malloc(dwSize);
	CopyMemory(buffer, lpAddress, dwSize);

	memset(lpAddress, 0x90, dwSize); // 0x90 is the opcode for NOP

	return buffer;
}

/* Relative jmp opcode is E9 */
VOID WriteRelativeJump(LPVOID from, LPVOID to) {
	DWORD oldProtection;
	DWORD dummy;

	VirtualProtect(from, 0x5, PAGE_EXECUTE_READWRITE, &oldProtection); // we need to write 5 bytes (size of jmp)

	*(PBYTE)from = 0xE9;
	*(PSIZE_T)((SIZE_T)from + 1) = GetRelativeAddress(from, to);

	VirtualProtect(from, 0x5, PAGE_EXECUTE_READWRITE, &dummy);
}


/* Absolute jmp opcode is EA */
VOID WriteAbsoluteJump(LPVOID from, LPVOID to) {
	DWORD oldProtection;
	DWORD dummy;

	VirtualProtect(from, 0x5, PAGE_EXECUTE_READWRITE, &oldProtection); // we need to write 5 bytes (size of jmp)

	*(PBYTE)from = 0xEA;
	*(PSIZE_T)((SIZE_T)from + 1) = (SIZE_T)to;

	VirtualProtect(from, 0x5, PAGE_EXECUTE_READWRITE, &dummy);
}

LPVOID GetPointerToTheReturnAddress(LPVOID func, SIZE_T sizeOfArguments) {
	return NULL;
}

BOOL hookVTable(LPVOID* VTable, LPVOID detour, int index) {
	DWORD oldProtection;
	DWORD dummy;

	VirtualProtect(VTable, (index + 1) * sizeof(LPVOID), PAGE_READWRITE, &oldProtection);

	VTable[index] = detour;

	VirtualProtect(VTable, (index + 1) * sizeof(LPVOID), oldProtection, &dummy);

	return TRUE;
}

BOOL hookFunc(LPVOID hookFunction, LPVOID detour, DWORD dwSize) {
	if (dwSize < 5) {
		return FALSE;
	}

	DWORD oldProtection;
	DWORD dummy;

	VirtualProtect(hookFunction, 0x5, PAGE_EXECUTE_READWRITE, &oldProtection); // we need to write 5 bytes (size of jmp)

	DWORD* ptr = (DWORD*)((DWORD)hookFunction + 1);
	printf("function jmp address: %ld\n", *ptr);

	FillWithNops(hookFunc, 0x5);

	*(SIZE_T*)hookFunction = 0xE9;

	SIZE_T relativeAddress = (SIZE_T)detour - (SIZE_T)hookFunction - 5;

	*(SIZE_T*)((SIZE_T)hookFunction + 1) = relativeAddress;

	VirtualProtect(hookFunction, dwSize, oldProtection, &dummy);

	return TRUE;
}

LPVOID* getVTable(LPVOID obj) {
	return *(LPVOID**)obj;
}


int func() {
	return 10;
}

int detour() {
	return 15;
}

int main() {

	cout << func() << endl;

	hookFunc(func, detour, 0x5);

	cout << func() << endl;
	
	getchar();
    return 0;
}

