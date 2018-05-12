/*
ADOBE SYSTEMS INCORPORATED
Copyright 1994 - 2008 Adobe Macromedia Software LLC
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/

#define _CRT_NO_VA_START_VALIDATION

#ifndef _H_Script
#define _H_Script

#include <windows.h>
#include "moastdif.h"
#include "xmmvalue.h"
#include "mmiutil.h"




#define kMoaErr_YesErr ((MoaError)1L)




// {EF754E9F-BE76-42D3-A7B2-5BB7947B5812}
DEFINE_GUID(CLSID_TStdXtra,
			0xef754e9f, 0xbe76, 0x42d3, 0xa7, 0xb2, 0x5b, 0xb7, 0x94, 0x7b, 0x58, 0x12);




EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(TStdXtra)
	PIMoaMmUtils2 pMoaMmUtilsInterface;
	PIMoaMmValue pMoaMmValueInterface;
	PIMoaDrPlayer pMoaDrPlayerInterface;
EXTERN_END_DEFINE_CLASS_INSTANCE_VARS




EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaRegister)
	EXTERN_DEFINE_METHOD(MoaError, Register, (PIMoaCache, PIMoaXtraEntryDict))
EXTERN_END_DEFINE_CLASS_INTERFACE




enum MODULE_HANDLE_SET {
	MODULE_HANDLE_SET_NULL,
	MODULE_HANDLE_SET_DIRECTOR_API,
	MODULE_HANDLE_SET_NET_LINGO_XTRA,
	MODULE_HANDLE_SET_SHOCKWAVE_3D_ASSET_XTRA
};




EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaMmXScript)
	EXTERN_DEFINE_METHOD(MoaError, Call, (PMoaDrCallInfo))
	private:
	EXTERN_DEFINE_METHOD(MoaError, XScrpGeneral, (PMoaDrCallInfo, MODULE_HANDLE_SET))
	EXTERN_DEFINE_METHOD(MoaError, XScrpGeneral, (PMoaDrCallInfo, PMoaLong, MODULE_HANDLE_SET))
	EXTERN_DEFINE_METHOD(MoaError, XScrpGeneral, (PMoaDrCallInfo, PMoaChar, MODULE_HANDLE_SET))
	EXTERN_DEFINE_METHOD(MoaError, XScrpSetExternalParam, (PMoaDrCallInfo, MODULE_HANDLE_SET))
EXTERN_END_DEFINE_CLASS_INTERFACE




class TextSection {
	private:
	LPVOID lpAddress = NULL;
	SIZE_T dwSize = 0;
	HANDLE currentProcess = INVALID_HANDLE_VALUE;
	DWORD lpflOldProtect = NULL;
	bool lpflOldProtectSet = false;
	bool flushed = true;

	public:
	bool unprotect();
	bool protect();
	bool flush();
	bool test(unsigned char* tested, size_t sizeofTested, DWORD testedAddress);
	bool write(DWORD writtenAddress);
	bool write(void* written, DWORD writtenAddress, bool call = false);
	TextSection(LPVOID lpAddress, SIZE_T dwSize, HANDLE currentProcess);
	~TextSection();
};




// Set or Force Property Written Addresses for 8.5
const DWORD WRITTEN_ADDRESS_SET_THE_MOVIE_PATH_85 = 0x000A8597;
const DWORD WRITTEN_ADDRESS_SET_THE_RUN_MODE_85 = 0x000A8D2F;
const DWORD WRITTEN_ADDRESS_SET_THE_PLATFORM_85 = 0x000A8CB3;
const DWORD WRITTEN_ADDRESS_SET_THE_MACHINE_TYPE_85 = 0x000C6E80;
const DWORD WRITTEN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_85 = 0x000D2338;
const DWORD WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_85 = 0x0001065E;
const DWORD WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_2_85 = 0x00007462;
const DWORD WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_3_85 = 0x000094AB;

// Set External Param Written Addresses for 8.5
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_85 = 0x000A7F40;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_85_2 = 0x000A7F72;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_85 = 0x000A804F;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_85_2 = 0x000A808E;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_85 = 0x000A7ED9;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_85_2 = 0x000A7F16;

// Disabled Handler Written Addresses for 8.5
const DWORD WRITTEN_ADDRESS_DISABLE_GO_TO_NET_MOVIE_85 = 0x000029A1;
const DWORD WRITTEN_ADDRESS_DISABLE_GO_TO_NET_PAGE_85 = 0x00002A2D;

// Bugfix Written Addresses for 8.5
const DWORD WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_85 = 0x000DC5D4;
const DWORD WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_85_2 = 0x000DC5E2;

// Set or Force Property Written Addresses for 10
const DWORD WRITTEN_ADDRESS_SET_THE_MOVIE_PATH_10 = 0x000AD982;
const DWORD WRITTEN_ADDRESS_SET_THE_RUN_MODE_10 = 0x000AC77E;
const DWORD WRITTEN_ADDRESS_SET_THE_PLATFORM_10 = 0x000AE32F;
const DWORD WRITTEN_ADDRESS_SET_THE_MACHINE_TYPE_10 = 0x000D40C0;
const DWORD WRITTEN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_10 = 0x000E0A34;
const DWORD WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_10 = 0x00012E66;
const DWORD WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_2_10 = 0x000074BD;
const DWORD WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_3_10 = 0x00009629;

// Set External Param Written Addresses for 10
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_10 = 0x000B248E;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_10_2 = 0x000B24C0;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_10 = 0x000B259D;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_10_2 = 0x000B25DC;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_10 = 0x000B1648;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_10_2 = 0x000B1685;

// Disabled Handler Written Addresses for 10
const DWORD WRITTEN_ADDRESS_DISABLE_GO_TO_NET_MOVIE_10 = 0x00001AA4;
const DWORD WRITTEN_ADDRESS_DISABLE_GO_TO_NET_PAGE_10 = 0x00001B30;

// Bugfix Written Addresses for 10
const DWORD WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_10 = 0x000F9165;
const DWORD WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_10_2 = 0x000F9173;

// Set or Force Property Written Addresses for 11.5
const DWORD WRITTEN_ADDRESS_SET_THE_MOVIE_PATH_115 = 0x000DEE8C;
const DWORD WRITTEN_ADDRESS_SET_THE_RUN_MODE_115 = 0x000DCDB2;
const DWORD WRITTEN_ADDRESS_SET_THE_RUN_MODE_115_2 = 0x000DCDBF;
const DWORD WRITTEN_ADDRESS_SET_THE_PLATFORM_115 = 0x000DF89E;
const DWORD WRITTEN_ADDRESS_SET_THE_MACHINE_TYPE_115 = 0x0010F901;
const DWORD WRITTEN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_115 = 0x00120E2D;
const DWORD WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_115 = 0x000179E2;
const DWORD WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_2_115 = 0x000088B0;
const DWORD WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_3_115 = 0x0000C27B;

// Set External Param Written Addresses for 11.5
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_115 = 0x000E15BF;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_115_2 = 0x000E15F3;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_115 = 0x000E162F;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_115_2 = 0x000E1670;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_115 = 0x000E144A;
const DWORD WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_115_2 = 0x000E1490;

// Disabled Handler Written Addresses for 11.5
const DWORD WRITTEN_ADDRESS_DISABLE_GO_TO_NET_MOVIE_115 = 0x00001968;
const DWORD WRITTEN_ADDRESS_DISABLE_GO_TO_NET_PAGE_115 = 0x00002090;

// Bugfix Written Addresses for 11.5
const DWORD WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_115 = 0x000D6AF8;
const DWORD WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_115_2 = 0x000D6B06;

// Set or Force Property Written Code Return Addresses 8.5
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_MOVIE_PATH_85 = 0x000AA182;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_RUN_MODE_85 = 0x000AA182;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_PLATFORM_85 = 0x000AA182;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_85 = 0x000D333D;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_FORCE_THE_EXIT_LOCK_85 = 0x0001152C;

// Set External Param Written Code Return Addresses 8.5
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_NAME_85 = 0x000A8F5B;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_85 = 0x000A906B;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_85 = 0x000A8EF8;

// Bugfix Written Code Return Addresses 8.5
const DWORD WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_85 = 0x000DD5D9;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_85_2 = 0x000DD5E7;

// Set or Force Property Written Code Return Addresses 10
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_MOVIE_PATH_10 = 0x000ADFD6;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_RUN_MODE_10 = 0x000AD951;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_PLATFORM_10 = 0x000AF35B;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_10 = 0x000E1A39;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_FORCE_THE_EXIT_LOCK_10 = 0x00013D34;

// Set External Param Written Code Return Addresses 10
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_NAME_10 = 0x000B34A9;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_10 = 0x000B35B9;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_10 = 0x000B2667;

// Bugfix Written Code Return Addresses 10
const DWORD WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_10 = 0x000FA16A;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_10_2 = 0x000FA178;

// Set or Force Property Written Code Return Addresses 11.5
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_MOVIE_PATH_115 = 0x000DFE91;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_RUN_MODE_115 = 0x000DDDB7;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_RUN_MODE_115_2 = 0x000DDDC6;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_PLATFORM_115 = 0x000E08A3;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_115 = 0x00121E32;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_FORCE_THE_EXIT_LOCK_115 = 0x000189E8;

// Set External Param Written Code Return Addresses 11.5
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_NAME_115 = 0x000E25DB;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_115 = 0x000E264C;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_115 = 0x000E246B;

// Bugfix Written Code Return Addresses 11.5
const DWORD WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_115 = 0x000D7AFD;
const DWORD WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_115_2 = 0x000D7B0B;

// Module Handles
HANDLE moduleHandleDirectorAPI = NULL;
bool moduleHandleDirectorAPISet = false;
HANDLE moduleHandleNetLingoXtra = NULL;
bool moduleHandleNetLingoXtraSet = false;
HANDLE moduleHandleShockwave3DAssetXtra = NULL;
bool moduleHandleShockwave3DAssetXtraSet = false;

// Module Handle Written Code Return Addresses
HANDLE moduleHandleWrittenCodeReturnAddressSetTheMoviePath = NULL;
HANDLE moduleHandleWrittenCodeReturnAddressSetTheRunMode = NULL;
HANDLE moduleHandleWrittenCodeReturnAddressSetTheRunMode2 = NULL;
HANDLE moduleHandleWrittenCodeReturnAddressSetThePlatform = NULL;
HANDLE moduleHandleWrittenCodeReturnAddressSetTheEnvironment_ShockMachine = NULL;
HANDLE moduleHandleWrittenCodeReturnAddressForceTheExitLock = NULL;
HANDLE moduleHandleWrittenCodeReturnAddressSetExternalParamName = NULL;
HANDLE moduleHandleWrittenCodeReturnAddressSetExternalParamValue = NULL;
HANDLE moduleHandleWrittenCodeReturnAddressSetExternalParamCount = NULL;
HANDLE moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList = NULL;
HANDLE moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList2 = NULL;

enum TESTED_SET {
	TESTED_SET_NULL,
	TESTED_SET_85,
	TESTED_SET_10,
	TESTED_SET_115
};
TESTED_SET testedDirectorAPISet = TESTED_SET_NULL;
TESTED_SET testedNetLingoXtraSet = TESTED_SET_NULL;
TESTED_SET testedShockwave3DAssetXtraSet = TESTED_SET_NULL;

// Properties
MoaChar theMoviePath[257];
MoaChar theRunMode[257];
int theRunModeSet = false;
MoaChar thePlatform[257];
MoaLong theMachineType = 0;
MoaLong theEnvironment_ShockMachine = 0;
MoaLong theExitLock = 0;

// External Params
int sizeofExternalParams = 0;
PMoaChar externalParams = new char[sizeofExternalParams];

__declspec(naked) void writtenSetTheMoviePath85() {
	__asm {
		// check we're in a Lingo script
		push eax
		mov eax, [ebp + 48h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000C9E6Ah // the address of Lingo's code
		pop eax
		jnz do_not_set

		// we can't just replace the old string because we won't have enough room
		// the old string is on the stack so it'll just go out of scope
		lea eax, theMoviePath
		jmp epilogue

		// this is just a repeat of some code we have to skip over when going back
		do_not_set:
		lea eax, [ebp - 0114h]

		epilogue:
		push [ebp + 14h]
		jmp [moduleHandleWrittenCodeReturnAddressSetTheMoviePath]
	}
}

__declspec(naked) void writtenSetTheRunMode85() {
	__asm {
		// we can get here multiple ways
		// check if the property is the one we want to replace
		cmp esi, 00000001h
		jnz do_not_set

		// check we're in a Lingo script
		push eax
		mov eax, [ebp + 48h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000C9E6Ah // the address of Lingo's code in 8.5
		pop eax
		jnz do_not_set

		lea eax, theRunMode
		jmp [moduleHandleWrittenCodeReturnAddressSetTheRunMode]

		do_not_set:
		lea eax, [ebp - 0114h]
		jmp [moduleHandleWrittenCodeReturnAddressSetTheRunMode]
	}
}

__declspec(naked) void writtenSetThePlatform85() {
	__asm {
		// check we're in a Lingo script
		push eax
		mov eax, [ebp + 48h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000C9E6Ah // the address of Lingo's code in 8.5
		pop eax
		jnz do_not_set

		lea eax, thePlatform
		jmp epilogue

		// again, this is just a repeat of some code we have to skip over when going back
		do_not_set:
		lea eax, [ebp - 0114h]

		epilogue:
		push [ebp + 14h]
		jmp [moduleHandleWrittenCodeReturnAddressSetThePlatform]
	}
}

__declspec(naked) void writtenSetTheMachineType85() {
	__asm {
		// this code is terribly hard to read, even for assembly
		// somebody let me know if there's equivalent code to accomplish the same but better

		// we can get here multiple ways again
		// however the value we need to check is at esp - 04h
		// but we want all our registers to remain untouched
		// so subtract four from esp
		sub esp, 04h
		push eax
		// check it's correct
		mov eax, [esp + 04h]
		cmp eax, 0000005Bh
		pop eax
		// we can't immediately add to esp or it'll affect the registers from our compare
		jnz do_not_set

		// add four back to esp after not jumping
		add esp, 04h

		// check we're in a Lingo script
		push eax
		mov eax, [esp + 0Ch] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000CA275h // the address of Lingo's code in 8.5
		pop eax
		jnz epilogue

		// get the address and replace the machine type
		push eax
		push ebx
		mov eax, [esp + 0Ch]
		mov ebx, theMachineType
		mov [eax + 20h], ebx
		pop ebx
		pop eax

		// prevent adding four more back to esp
		jmp epilogue

		do_not_set:
		// add four back to esp after jumping
		add esp, 04h
		
		// epilogue code
		// we're at the end of the subroutine anyway so just return normally
		epilogue:
		pop edi
		pop esi
		retn 8
	}
}

__declspec(naked) void writtenSetTheEnvironment_ShockMachine85() {
	__asm {
		// push the arguments for the subroutine we're going to call
		push eax
		push esi

		// check the property is the one we care about
		push eax
		mov eax, [ebp + 10h] // the property's address we're going to pass
		cmp eax, 00000353h // shockMachine 8.5
		pop eax
		jnz do_not_set

		// check the subroutine we're going to call is the one we care about
		push eax
		mov eax, [ecx + 18h] // the subroutine's address we're going to call
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000BFBDDh // the address of the subroutine we care about in 8.5
		pop eax
		jnz do_not_set

		// check we're in a Lingo script
		push eax
		mov eax, [ebp + 34h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000C9F40h // the address of Lingo's code in 8.5
		pop eax
		jnz do_not_set

		// call the subroutine we do care about
		call [ecx + 18h]

		// the call has completed, set the variable it returned to
		push eax
		mov eax, theEnvironment_ShockMachine
		mov [ebp + 24h], eax
		pop eax

		// avoid calling the subroutine a second time
		jmp [moduleHandleWrittenCodeReturnAddressSetTheEnvironment_ShockMachine]

		do_not_set:
		// call a subroutine we don't care about
		call [ecx + 18h]

		jmp [moduleHandleWrittenCodeReturnAddressSetTheEnvironment_ShockMachine]
	}
}

__declspec(naked) void writtenForceTheExitLock() {
	__asm {
		// we want all of Director to force this exit lock, so we don't need to check if we're in a Lingo script
		mov ecx, theExitLock
		jmp [moduleHandleWrittenCodeReturnAddressForceTheExitLock]
	}
}

__declspec(naked) void writtenForceTheExitLock23() {
	__asm {
		// in addition to telling Lingo what the exitLock is, we also need to actually have it have an effect
		mov eax, theExitLock
		retn 04h
	}
}

__declspec(naked) void writtenSetExternalParamName8510() {
	__asm {
		// this is for a Lingo handler, so we don't need to check if we're in a Lingo script
		mov eax, externalParams
		jmp [moduleHandleWrittenCodeReturnAddressSetExternalParamName]
	}
}

__declspec(naked) void writtenSetExternalParamValue8510() {
	__asm {
		// we don't need to check if we're in a Lingo script for the same reason as above
		mov eax, externalParams
		jmp [moduleHandleWrittenCodeReturnAddressSetExternalParamValue]
	}
}

__declspec(naked) void writtenSetExternalParamCount8510() {
	__asm {
		// ditto
		push esi
		mov eax, externalParams
		jmp [moduleHandleWrittenCodeReturnAddressSetExternalParamCount]
	}
}

__declspec(naked) void writtenSetTheMoviePath10() {
	__asm {
		// check we're in a Lingo script
		push eax
		mov eax, [ebp + 0178h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000D7059h // the address of Lingo's code in 10
		pop eax
		jnz do_not_set

		// this is done first in 10, not that I think it matters
		push [ebp + 14h]
		lea eax, theMoviePath
		jmp [moduleHandleWrittenCodeReturnAddressSetTheMoviePath]

		do_not_set:
		lea eax, [ebp - 0114h]
		jmp [moduleHandleWrittenCodeReturnAddressSetTheMoviePath]
	}
}

__declspec(naked) void writtenSetTheRunMode10() {
	__asm {
		// we can get here multiple ways
		// check if the property is the one we want to replace
		cmp edi, 00000001h
		jnz do_not_set

		// check we're in a Lingo script
		push eax
		mov eax, [ebp + 0178h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000D7059h // the address of Lingo's code in 10
		pop eax
		jnz do_not_set

		// just like in 8.5, there's no additional push here
		lea eax, theRunMode
		jmp [moduleHandleWrittenCodeReturnAddressSetTheRunMode]

		do_not_set:
		// or here
		lea eax, [ebp - 0114h]
		jmp [moduleHandleWrittenCodeReturnAddressSetTheRunMode]
	}
}

__declspec(naked) void writtenSetThePlatform10() {
	__asm {
		// check we're in a Lingo script
		push eax
		mov eax, [ebp + 4Ch] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000D7059h // the address of Lingo's code in 10
		pop eax
		jnz do_not_set

		// the other push isn't done here in 10
		lea eax, thePlatform
		jmp [moduleHandleWrittenCodeReturnAddressSetThePlatform]

		do_not_set:
		lea eax, [ebp - 0114h]
		jmp [moduleHandleWrittenCodeReturnAddressSetThePlatform]
	}
}

__declspec(naked) void writtenSetTheMachineType10() {
	__asm {
		// we can get here multiple ways again
		// however the value we need to check is at esp - 04h
		// but we want all our registers to remain untouched
		// so subtract four from esp
		sub esp, 04h
		push eax
		// check it's correct
		mov eax, [esp + 04h]
		cmp eax, 0000005Bh
		pop eax
		// we can't immediately add to esp or it'll affect the registers from our compare
		jnz do_not_set

		// add four back to esp after not jumping
		add esp, 04h

		// check we're in a Lingo script
		push eax
		mov eax, [esp + 0Ch] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000D7463h // the address of Lingo's code in 10
		pop eax
		jnz epilogue

		// get the address and replace the machine type
		push eax
		push ebx
		mov eax, [esp + 0Ch]
		mov ebx, theMachineType
		mov [eax + 20h], ebx
		pop ebx
		pop eax

		// prevent adding four more back to esp
		jmp epilogue

		do_not_set:
		// add four back to esp after jumping
		add esp, 04h

		// epilogue code
		// we're at the end of the subroutine anyway so just return normally
		epilogue:
		pop edi
		pop esi
		retn 8
	}
}

__declspec(naked) void writtenSetTheEnvironment_ShockMachine10() {
	__asm {
		// push the arguments for the subroutine we're going to call
		push eax
		push esi

		// check the property is the one we care about
		push eax
		mov eax, [ebp + 10h] // the property's address we're going to pass
		cmp eax, 00000372h // shockMachine 10
		pop eax
		jnz do_not_set

		// check the subroutine we're going to call is the one we care about
		push eax
		mov eax, [ecx + 18h] // the subroutine's address we're going to call
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000CCBDDh // the address of the subroutine we care about in 10
		pop eax
		jnz do_not_set

		// check we're in a Lingo script
		push eax
		mov eax, [ebp + 34h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 000D712Fh // the address of Lingo's code in 10
		pop eax
		jnz do_not_set

		// call the subroutine we do care about
		call [ecx + 18h]

		// the call has completed, set the variable it returned to
		push eax
		mov eax, theEnvironment_ShockMachine
		mov [ebp + 24h], eax
		pop eax

		// avoid calling the subroutine a second time
		jmp [moduleHandleWrittenCodeReturnAddressSetTheEnvironment_ShockMachine]

		do_not_set:
		// call a subroutine we don't care about
		call [ecx + 18h]

		jmp [moduleHandleWrittenCodeReturnAddressSetTheEnvironment_ShockMachine]
	}
}

__declspec(naked) void writtenSetTheMoviePath115() {
	__asm {
		// check we're in a Lingo script
		push eax
		mov eax, [esp + 0894h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 00112FDDh // the address of Lingo's code in 11.5
		pop eax
		push eax // 11 does this
		jnz do_not_set

		// we can't just replace the old string because we won't have enough room
		// the old string is on the stack so it'll just go out of scope
		// these are not Unicode strings in 11.5, despite 11.5 being Unicode compatible, somehow
		lea ecx, theMoviePath
		jmp [moduleHandleWrittenCodeReturnAddressSetTheMoviePath]

		// this is just a repeat of some code we have to skip over when going back
		do_not_set:
		lea ecx, [esp + 24h]

		jmp [moduleHandleWrittenCodeReturnAddressSetTheMoviePath]
	}
}

__declspec(naked) void writtenSetTheRunMode115() {
	__asm {
		// we can get here multiple ways
		// check if the property is the one we want to replace
		push edx
		mov edx, [esp - 04h]
		mov theRunModeSet, edx
		pop edx

		lea edx, [esp + 1Ch]
		push edx
		jmp [moduleHandleWrittenCodeReturnAddressSetTheRunMode]
	}
}

__declspec(naked) void writtenSetTheRunMode1152() {
	__asm {
		// we can get here multiple ways
		// check if the property is the one we want to replace
		push edx
		mov edx, theRunModeSet
		cmp edx, 00000001h
		pop edx
		jnz epilogue

		// check we're in a Lingo script
		push eax
		mov eax, [esp + 0890h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 00112FDDh // the address of Lingo's code in 11.5
		pop eax
		jnz epilogue

		lea eax, theRunMode

		epilogue:
		// eax already contains the correct thing
		push eax
		push esi
		mov eax, 000F83C0h
		add eax, moduleHandleDirectorAPI
		call eax
		jmp [moduleHandleWrittenCodeReturnAddressSetTheRunMode2]
	}
}

__declspec(naked) void writtenSetThePlatform115() {
	__asm {
		// check we're in a Lingo script
		push eax
		mov eax, [esp + 464h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 00112FDDh // the address of Lingo's code in 11.5
		pop eax
		jnz do_not_set

		lea ecx, thePlatform
		// a push is done here in 11.5
		push ecx
		jmp [moduleHandleWrittenCodeReturnAddressSetThePlatform]

		do_not_set:
		lea ecx, [esp + 18h]
		// and as such here as well
		push ecx
		jmp [moduleHandleWrittenCodeReturnAddressSetThePlatform]
	}
}

__declspec(naked) void writtenSetTheMachineType115() {
	__asm {
		// slightly different stack than 8.5/10 here
		push eax
		mov eax, [esp + 50h]
		cmp eax, 0000005Bh
		pop eax
		jnz epilogue

		// check we're in a Lingo script
		push eax
		mov eax, [esp + 48h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 001133F5h // the address of Lingo's code in 10
		pop eax
		jnz epilogue

		// get the address and replace the machine type
		push ebx
		mov ebx, theMachineType
		mov [esi + 20h], ebx
		pop ebx

		// epilogue code
		// we're at the end of the subroutine anyway so just return normally
		epilogue:
		pop ebx
		mov esp, ebp
		pop ebp
		retn 8
	}
}

__declspec(naked) void writtenSetTheEnvironment_ShockMachine115() {
	__asm {
		// push the arguments for the subroutine we're going to call
		push eax
		push esi

		// check the property is the one we care about
		cmp ebp, 00000372h // shockMachine 11.5
		jnz do_not_set

		// check the subroutine we're going to call is the one we care about
		push eax
		mov eax, [edx + 18h] // the subroutine's address we're going to call (11.5 has it in EDX not ECX)
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 0010A280h // the address of the subroutine we care about in 11.5
		pop eax
		jnz do_not_set

		// check we're in a Lingo script
		push eax
		mov eax, [esp + 48h] // the return address of the subroutine we're currently in
		sub eax, moduleHandleDirectorAPI // regardless of where the module is loaded
		cmp eax, 001130ABh // the address of Lingo's code in 11.5
		pop eax
		jnz do_not_set

		// call the subroutine we do care about
		call [edx + 18h]

		// the call has completed, set the variable it returned to
		push eax
		mov eax, theEnvironment_ShockMachine
		mov [edi + 04h], eax
		pop eax

		// avoid calling the subroutine a second time
		jmp [moduleHandleWrittenCodeReturnAddressSetTheEnvironment_ShockMachine]

		do_not_set:
		// call a subroutine we don't care about
		call [edx + 18h]

		jmp [moduleHandleWrittenCodeReturnAddressSetTheEnvironment_ShockMachine]
	}
}

__declspec(naked) void writtenSetExternalParamName115() {
	__asm {
		// this is for a Lingo handler, so we don't need to check if we're in a Lingo script
		push esi
		mov eax, externalParams
		jmp [moduleHandleWrittenCodeReturnAddressSetExternalParamName]
	}
}

__declspec(naked) void writtenSetExternalParamValue115() {
	__asm {
		// we don't need to check if we're in a Lingo script for the same reason as above
		push ebx
		push esi
		mov eax, externalParams
		jmp [moduleHandleWrittenCodeReturnAddressSetExternalParamValue]
	}
}

__declspec(naked) void writtenSetExternalParamCount115() {
	__asm {
		// ditto
		push ebx
		push edi
		mov eax, externalParams
		jmp [moduleHandleWrittenCodeReturnAddressSetExternalParamCount]
	}
}

__declspec(naked) void writtenDisableGoToNetThing() {
	__asm {
		retn 8
	}
}

__declspec(naked) void writtenBugfixShockwave3DBadDriverList85() {
	__asm {
		mov esi, [ebp - 4]
		add esi, 0109h
		jmp [moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList]
	}
}

__declspec(naked) void writtenBugfixShockwave3DBadDriverList852() {
	__asm {
		push 0104h
		push edi
		mov [ebp - 14h], edi
		jmp [moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList2]
	}
}

__declspec(naked) void writtenBugfixShockwave3DBadDriverList10() {
	__asm {
		mov esi, [ebp - 4]
		add esi, 0109h
		jmp [moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList]
	}
}

__declspec(naked) void writtenBugfixShockwave3DBadDriverList102() {
	__asm {
		push 0104h
		push edi
		mov [ebp - 14h], edi
		jmp [moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList2]
	}
}

__declspec(naked) void writtenBugfixShockwave3DBadDriverList115() {
	__asm {
		mov edi, [ebp - 1Ch]
		add edi, 0109h
		jmp [moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList]
	}
}

__declspec(naked) void writtenBugfixShockwave3DBadDriverList1152() {
	__asm {
		push 104h
		push ebx
		mov [ebp - 14h], ebx
		mov [ebp - 1Ch], edi
		jmp [moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList2]
	}
}

#endif