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

BOOL hookFunc(LPVOID hookFunction, LPVOID detour, DWORD dwSize) {
	if (dwSize < 5) {
		return FALSE;
	}

	DWORD oldProtection;
	DWORD dummy;

	VirtualProtect(hookFunction, dwSize, PAGE_EXECUTE_READWRITE, &oldProtection);

	DWORD* ptr = (DWORD*)((DWORD)hookFunction + 1);
	printf("function jmp address: %ld\n", *ptr);

	hook_info.dwSize = dwSize;
	hook_info.data = (BYTE*)malloc(dwSize); // reserve memory for the array#
	hook_info.lpAddress = (LPVOID)((PBYTE)hookFunction + dwSize);

	CopyMemory(hook_info.data, hookFunction, dwSize);
	memset(hookFunction, 0x90, dwSize);

	*(SIZE_T*)hookFunction = 0xE9;

	SIZE_T relativeAddress = (SIZE_T)detour - (SIZE_T)hookFunction - 5;
	
    *(SIZE_T*)((SIZE_T)hookFunction + 1) = relativeAddress;
	
	VirtualProtect(hookFunction, dwSize, oldProtection, &dummy);
	
	return TRUE;
}

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

}

/* Returns a Byte array containg the bytes that are overriden */
PBYTE FillWithNops(LPVOID lpAddress, DWORD dwSize) {
	PBYTE buffer = (PBYTE)malloc(dwSize);
	CopyMemory(buffer, lpAddress, dwSize);

	return buffer;
}

/* Returns a Byte array containing the bytes that are overriden */
VOID WriteRelativeJump(LPVOID from, LPVOID to) {

}


VOID WriteAbsoluteJump(LPVOID from, LPVOID to) {

}

LPVOID GetPointerToTheReturnAddress(LPVOID func, SIZE_T sizeOfArguments) {

}

BOOL hookVTable(LPVOID* VTable, LPVOID detour, int index) {
	DWORD oldProtection;
	DWORD dummy;

	VirtualProtect(VTable, (index + 1) * sizeof(LPVOID), PAGE_READWRITE, &oldProtection);

	VTable[index] = detour;

	VirtualProtect(VTable, (index + 1) * sizeof(LPVOID), oldProtection, &dummy);

	return TRUE;
}


LPVOID* getVTable(LPVOID obj) {
	return *(LPVOID**)obj;
}


int main() {

	getchar();
    return 0;
}

