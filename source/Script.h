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
#include <Dbghelp.h>
#include <string.h>

#include "xmmvalue.h"




enum MODULE {
	MODULE_DIRECTOR_API,
	MODULE_NET_LINGO_XTRA,
	MODULE_SHOCKWAVE_3D_ASSET_XTRA
};

enum MODULE_DIRECTOR_VERSION {
	MODULE_DIRECTOR_INCOMPATIBLE = -1,
	MODULE_DIRECTOR_8,
	MODULE_DIRECTOR_85,
	MODULE_DIRECTOR_851,
	MODULE_DIRECTOR_9,
	MODULE_DIRECTOR_10,
	MODULE_DIRECTOR_101,
	MODULE_DIRECTOR_1011,
	MODULE_DIRECTOR_11,
	MODULE_DIRECTOR_1103,
	MODULE_DIRECTOR_115,
	MODULE_DIRECTOR_1158,
	MODULE_DIRECTOR_1159,
	MODULE_DIRECTOR_12
};




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

EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaMmXScript)
	EXTERN_DEFINE_METHOD(MoaError, Call, (PMoaDrCallInfo))
	private:
	EXTERN_DEFINE_METHOD(MoaError, XScrpExtender, (PMoaDrCallInfo, MODULE, PIMoaDrMovie))
	EXTERN_DEFINE_METHOD(MoaError, XScrpExtender, (PMoaDrCallInfo, MODULE))
	EXTERN_DEFINE_METHOD(MoaError, XScrpExtender, (PMoaDrCallInfo, MODULE, PMoaLong))
	EXTERN_DEFINE_METHOD(MoaError, XScrpExtender, (PMoaDrCallInfo, MODULE, MoaLong, PMoaChar))
	EXTERN_DEFINE_METHOD(MoaError, XScrpSetExternalParam, (PMoaDrCallInfo, MODULE))
EXTERN_END_DEFINE_CLASS_INTERFACE




inline size_t stringSize(const char* string) {
	return strlen(string) + 1;
}

inline bool stringsEqual(const char* leftHandSide, const char* rightHandSide) {
	return !strcmp(leftHandSide, rightHandSide);
}

inline bool memoryEqual(const void* buffer, const void* buffer2, size_t bufferSize) {
	return !memcmp(buffer, buffer2, bufferSize);
}

bool shiftMemory(size_t bufferSize, const void* buffer, size_t sourceSize, const void* source, unsigned int shift, bool direction) {
	if (source < buffer || (char*)source + sourceSize > (char*)buffer + bufferSize) {
		return false;
	}
	
	size_t destinationSize = (char*)buffer + bufferSize - source;
	char* destination = (char*)source;
	if (!direction) {
		destination -= shift;
	} else {
		destination += shift;
	}
	
	if (destination < buffer || destination + sourceSize > (char*)buffer + bufferSize) {
		return false;
	}
	
	return !memmove_s(destination, destinationSize, source, sourceSize);
}




inline bool callLingoQuit(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface) {
	// we can't just use Win32 API's TerminateProcess because it causes a long pause before closing the movie improperly
	// so we use Lingo's Quit handler
	MoaError err = kMoaErr_NoErr;
	MoaMmSymbol quitSymbol;

	err = pMoaMmValueInterface->StringToSymbol("Quit", &quitSymbol);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaDrMovieInterface->CallHandler(quitSymbol, 0, NULL, NULL);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	return true;
}

inline bool callLingoAlert(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar message) {
	// we can't just use Win32 API's MessageBox or it'll cause graphical glitches from the movie not being paused
	// so we use Lingo's Alert handler (as per the XDK's recommendation)
	MoaError err = kMoaErr_NoErr;
	MoaMmSymbol alertSymbol;
	MoaMmValue messageValue = kVoidMoaMmValueInitializer;

	err = pMoaMmValueInterface->StringToSymbol("Alert", &alertSymbol);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaMmValueInterface->StringToValue(message, &messageValue);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &messageValue, NULL);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	return true;
}

inline bool callLingoAlertXtraMissing(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar message) {
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, message)) {
		return false;
	}
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "An Xtra that the LeechProtectionRemovalHelp Xtra requires, such as the NetLingo Xtra or Shockwave 3D Asset Xtra, is missing.")) {
		return false;
	}
	return true;
}

inline bool callLingoAlertComponentCorrupted(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar message) {
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, message)) {
		return false;
	}
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "A Component that the LeechProtectionRemovalHelp Xtra requires is corrupted, garbled or tampered with. Please use the official Components provided by Macromedia/Adobe.")) {
		return false;
	}
	return true;
}

inline bool callLingoAlertIncompatibleDirectorVersion(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar message) {
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, message)) {
		return false;
	}
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "The LeechProtectionRemovalHelp Xtra has determined it is incompatible with this Director version.")) {
		return false;
	}
	return true;
}

inline bool callLingoAlertAntivirus(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar message) {
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, message)) {
		return false;
	}
	if (!callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "Your Antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.")) {
		return false;
	}
	return true;
}

inline bool setLingoSafePlayer(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, MoaLong safePlayer) {
	MoaError err = kMoaErr_NoErr;
	MoaMmSymbol safePlayerSymbol;
	MoaMmValue safePlayerValue = kVoidMoaMmValueInitializer;

	err = pMoaMmValueInterface->StringToSymbol("SafePlayer", &safePlayerSymbol);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaMmValueInterface->IntegerToValue(safePlayer, &safePlayerValue);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaDrMovieInterface->SetProp(safePlayerSymbol, &safePlayerValue);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	return true;
}

inline bool callLingoVoidPNewXtra(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, ConstPMoaChar whichXtra) {
	MoaError err = kMoaErr_NoErr;
	MoaMmSymbol xtraSymbol;
	MoaMmValue whichXtraValue = kVoidMoaMmValueInitializer;
	MoaMmValue xtraResult = kVoidMoaMmValueInitializer;
	MoaMmSymbol newSymbol;
	MoaMmValue newResult = kVoidMoaMmValueInitializer;
	MoaMmSymbol voidPSymbol;
	MoaMmValue voidPResult = kVoidMoaMmValueInitializer;
	MoaLong voidP = 0;

	err = pMoaMmValueInterface->StringToSymbol("Xtra", &xtraSymbol);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaMmValueInterface->StringToValue(whichXtra, &whichXtraValue);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaDrMovieInterface->CallHandler(xtraSymbol, 1, &whichXtraValue, &xtraResult);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaMmValueInterface->StringToSymbol("New", &newSymbol);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaDrMovieInterface->CallHandler(newSymbol, 1, &xtraResult, &newResult);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaMmValueInterface->StringToSymbol("VoidP", &voidPSymbol);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaDrMovieInterface->CallHandler(voidPSymbol, 1, &newResult, &voidPResult);
	if (err != kMoaErr_NoErr) {
		return false;
	}
	err = pMoaMmValueInterface->ValueToInteger(&voidPResult, &voidP);
	if (err != kMoaErr_NoErr || voidP) {
		return false;
	}
	return true;
}




inline DWORD createExtendedCodeAddress(HANDLE moduleHandle, DWORD address) {
	return (DWORD)moduleHandle + address;
}

bool getSectionAddressAndSize(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD virtualAddress, DWORD virtualSize) {
	if (!moduleHandle) {
		//callLingoAlertXtraMissing(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Module Handle");
		return false;
	}
	PIMAGE_NT_HEADERS imageNtHeader = ImageNtHeader(moduleHandle);
	if (!imageNtHeader) {
		//callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Image NT Header");
		return false;
	}
	PIMAGE_SECTION_HEADER imageSectionHeader = (PIMAGE_SECTION_HEADER)(imageNtHeader + 1);
	if (!imageSectionHeader) {
		imageNtHeader = NULL;
		//callLingoAlert(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Image Section Header");
		return false;
	}
	for (WORD i = 0;i < imageNtHeader->FileHeader.NumberOfSections;i++) {
		if (virtualAddress >= (DWORD)moduleHandle + imageSectionHeader->VirtualAddress && virtualAddress + virtualSize <= (DWORD)moduleHandle + imageSectionHeader->VirtualAddress + imageSectionHeader->Misc.VirtualSize) {
			imageNtHeader = NULL;
			imageSectionHeader = NULL;
			return true;
		}
		imageSectionHeader++;
	}
	// not dangerous, so let the caller decide whether or not to quit
	imageNtHeader = NULL;
	imageSectionHeader = NULL;
	return false;
}

bool unprotectCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD virtualAddress, DWORD virtualSize, DWORD &lpflOldProtect) {
	if (!getSectionAddressAndSize(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize)) {
		return false;
	}

	if (!VirtualProtect((LPVOID)virtualAddress, virtualSize, PAGE_EXECUTE_READWRITE, &lpflOldProtect) || !virtualAddress || !virtualSize) {
		return false;
	}

	// get Basic Memory Information
	MEMORY_BASIC_INFORMATION memoryBasicInformation;
	if (VirtualQuery((LPCVOID)virtualAddress, &memoryBasicInformation, sizeof(memoryBasicInformation)) != sizeof(memoryBasicInformation)
		|| !memoryBasicInformation.Protect
		|| memoryBasicInformation.Protect & PAGE_NOACCESS
		|| memoryBasicInformation.Protect & PAGE_EXECUTE) {
		// dangerous - we unprotected the code but we can't query it, so quit
		callLingoAlertAntivirus(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Basic Memory Information");
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return true;
}

bool protectCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD virtualAddress, DWORD virtualSize, DWORD &lpflOldProtect) {
	if (!getSectionAddressAndSize(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize)) {
		callLingoAlertComponentCorrupted(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Section Address And Size");
		return false;
	}

	if (!lpflOldProtect || !VirtualProtect((LPVOID)virtualAddress, virtualSize, lpflOldProtect, &lpflOldProtect)) {
		callLingoAlertAntivirus(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Protect Code");
		return false;
	}
	return true;
}

bool flushCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD virtualAddress, DWORD virtualSize) {
	if (!getSectionAddressAndSize(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize)) {
		callLingoAlertComponentCorrupted(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Get Section Address And Size");
		return false;
	}

	if (!FlushInstructionCache(GetCurrentProcess(), (LPCVOID)virtualAddress, virtualSize)) {
		callLingoAlertAntivirus(pMoaMmValueInterface, pMoaDrMovieInterface, "Failed to Flush Code");
		return false;
	}
	return true;
}

bool testCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD relativeVirtualAddress, DWORD virtualSize, unsigned char code[]) {
	if (!moduleHandle) {
		return false;
	}
	DWORD virtualAddress = (DWORD)moduleHandle + relativeVirtualAddress;
	DWORD lpflOldProtect = 0;

	if (!unprotectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		return false;
	}

	bool result = memoryEqual((const void*)virtualAddress, (const void*)code, virtualSize);

	if (!protectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return result;
}

bool extendCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD relativeVirtualAddress, void* code, bool call = false) {
	if (!moduleHandle) {
		return false;
	}
	DWORD virtualAddress = (DWORD)moduleHandle + relativeVirtualAddress;
	DWORD virtualSize = 5;
	DWORD lpflOldProtect = 0;

	if (!unprotectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		return false;
	}

	if (!call) {
		*(PBYTE)virtualAddress = 0xE9;
	} else {
		*(PBYTE)virtualAddress = 0x58;
	}

	*(PDWORD)((PBYTE)virtualAddress + 1) = (DWORD)code - virtualAddress - virtualSize;

	if (!flushCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize)) {
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}

	if (!protectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return true;
}

bool extendCode(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, HANDLE moduleHandle, DWORD relativeVirtualAddress) {
	if (!moduleHandle) {
		return false;
	}
	DWORD virtualAddress = (DWORD)moduleHandle + relativeVirtualAddress;
	DWORD virtualSize = 1;
	DWORD lpflOldProtect = 0;

	if (!unprotectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		return false;
	}

	*(PBYTE)virtualAddress = 0x90;

	if (!flushCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize)) {
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}

	if (!protectCode(pMoaMmValueInterface, pMoaDrMovieInterface, moduleHandle, virtualAddress, virtualSize, lpflOldProtect)) {
		callLingoQuit(pMoaMmValueInterface, pMoaDrMovieInterface);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	return true;
}




// Properties
const size_t THE_MOVIE_PATH_SIZE = 256;
MoaChar theMoviePath[THE_MOVIE_PATH_SIZE] = "";

const size_t THE_MOVIE_NAME_SIZE = 256;
MoaChar theMovieName[THE_MOVIE_NAME_SIZE] = "";

MoaLong theEnvironment_shockMachine = 0;

const size_t THE_ENVIRONMENT_SHOCK_MACHINE_VERSION_SIZE = 256;
MoaChar theEnvironment_shockMachineVersion[THE_ENVIRONMENT_SHOCK_MACHINE_VERSION_SIZE] = "";

const size_t THE_PLATFORM_SIZE = 256;
MoaChar thePlatform[THE_PLATFORM_SIZE] = "";

const size_t THE_RUN_MODE_SIZE = 256;
MoaChar theRunMode[THE_RUN_MODE_SIZE] = "";

const size_t THE_ENVIRONMENT_PRODUCT_BUILD_VERSION_SIZE = 256;
MoaChar theEnvironment_productBuildVersion[THE_ENVIRONMENT_PRODUCT_BUILD_VERSION_SIZE] = "";

const size_t THE_PRODUCT_VERSION_SIZE = 256;
MoaChar theProductVersion[THE_PRODUCT_VERSION_SIZE] = "";

const size_t THE_ENVIRONMENT_OS_VERSION_SIZE = 256;
MoaChar theEnvironment_osVersion[THE_ENVIRONMENT_OS_VERSION_SIZE] = "";

MoaLong theMachineType = 0;

// External Params
size_t externalParamsSize = 0;
PMoaChar externalParams = NULL;

MoaLong theExitLock = 0;

MoaLong theSafePlayer = 0;




bool extender(PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, MoaMmSymbol methodSelector, MODULE module);

#endif