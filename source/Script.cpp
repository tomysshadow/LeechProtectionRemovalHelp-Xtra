/*
ADOBE SYSTEMS INCORPORATED
Copyright 1994 - 2008 Adobe Macromedia Software LLC
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source oSetTher than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/

#define _CRT_NO_VA_START_VALIDATION
#define INITGUID 1

#include <windows.h>
#include <Dbghelp.h>
#include "script.h"

#include <string.h>
#include <stdlib.h>
#include "xclassver.h"
#include "moatry.h"

#include "driservc.h"
#include "drivalue.h"

#include "mmivalue.h"
#include "mmillist.h"
#include "mmiplist.h"
#include "mmidate.h"
#include "mmiclr.h"




bool TextSection::unprotect() {
	if (!VirtualProtect(lpAddress, dwSize, PAGE_EXECUTE_READWRITE, &lpflOldProtect) || !lpAddress || !dwSize) {
		return false;
	}
	lpflOldProtectSet = true;
	return true;
}

bool TextSection::protect() {
	if (!lpflOldProtectSet || !VirtualProtect(lpAddress, dwSize, lpflOldProtect, &lpflOldProtect)) {
		return false;
	}
	return true;
}

bool TextSection::flush() {
	if (!flushed) {
		flushed = true;
		if (!FlushInstructionCache(currentProcess, lpAddress, dwSize)) {
			flushed = false;
		}
	}
	return flushed;
}

bool TextSection::test(unsigned char* tested, size_t sizeofTested, DWORD testedAddress) {
	// set Test Position absolutely
	testedAddress = (DWORD)lpAddress + testedAddress;
	if (testedAddress + sizeofTested > (DWORD)lpAddress + dwSize) {
		return false;
	}

	// get Basic Memory Information
	MEMORY_BASIC_INFORMATION memoryBasicInformation;
	if (VirtualQuery((LPCVOID)testedAddress, &memoryBasicInformation, sizeof(memoryBasicInformation)) != sizeof(memoryBasicInformation)
		|| !memoryBasicInformation.Protect
		|| memoryBasicInformation.Protect & PAGE_NOACCESS
		|| memoryBasicInformation.Protect & PAGE_EXECUTE) {
		return false;
	}
	return !memcmp((const void*)testedAddress, tested, sizeofTested); // false means they are the same
}

bool TextSection::write(DWORD writtenAddress) {
	const BYTE NOP = 0x90;
	const char NOP_LENGTH = 1;

	// set Write Position absolutely
	writtenAddress = (DWORD)lpAddress + writtenAddress;
	if (writtenAddress + NOP_LENGTH > (DWORD)lpAddress + dwSize) {
		return false;
	}

	// write code
	*(BYTE*)writtenAddress = NOP;
	flushed = false;
	return true;
}

bool TextSection::write(void* written, DWORD writtenAddress, bool call) {
	const BYTE JMP = 0xE9;
	const BYTE CALL = 0x58;
	const char LENGTH = 5;

	// set Write Position absolutely
	writtenAddress = (DWORD)lpAddress + writtenAddress;
	if (writtenAddress + LENGTH > (DWORD)lpAddress + dwSize) {
		return false;
	}
	// write code
	// for the sake of simplicity, we do not account for the memory instruction(s) we're replacing being a different length
	// that is to be dealt with on a case by case basis during implementation
	if (!call) {
		*(BYTE*)writtenAddress = JMP;
	} else {
		*(BYTE*)writtenAddress = CALL;
	}
	*(DWORD*)((BYTE*)writtenAddress + 1) = (DWORD)written - writtenAddress - LENGTH;
	flushed = false;
	return true;
}

TextSection::TextSection(LPVOID lpAddress, SIZE_T dwSize, HANDLE currentProcess):
	lpAddress(lpAddress),
	dwSize(dwSize),
	currentProcess(currentProcess) {
}

TextSection::~TextSection() {
	flush();
	protect();
	lpAddress = NULL;
}




/*******************************************************************************
 * SCRIPTING XTRA MESSAGE TABLE DESCRIPTION.
 *
 * the general format is:
 * xtra <nameOfXtra>
 * new object me [ args ... ]
 * <oSetTherHandlerDefintions>
 * --
 * the first line must give the name of the Scripting xtra.
 * the remaining lines give the names of the handlers that this xtra implements
 * along with the required types of the arguments for each handler.
 * 
 * -- Pre-defined handler new 
 * the new handler will be called when a child object is created, 
 * the first argument is always the child object and you defined any remaining arguments.
 * the new handler is the place to initialize any memory required by the child object.
 * 
 * -- Simple Handler Definitions
 * Each handler definition line is format like this:
 * <handlerName> <argType1> <argName1>, <argType2> <argName2> ...
 * the first word is the handler name. Following this are types description for
 * the argument to the handler, each separated by a comma.
 * the argument name <argName>, may be omited.
 * Permited argument types are: 
 * 	integer 
 * 	float
 * 	string
 * 	symbol
 * 	object
 * 	any
 * 	*
 * For integer, float, string, symbol, and object, the type of the argument must 
 * match. the type any means allow any type. the asterisk (*) means any number and 
 * any type of arguments.
 * 
 * the first argument is the child object and is always declared to be of type object.
 * 
 * -- Global Handlers
 * An asterisk (*) preceeding the handler name signifies a global handler.
 * This handler is at the global scope level and can be called from any
 * movie.
 * 
 * -- Xtra level handlers
 * A plus (+) preceeding the handler name signifies an Xtra level handler. 
 * This kind of handler can be called directly from the Xtra reference,
 * without creating a child object.
 * 
 * the enumerated list that follows must correspond directly with the msgTable 
 * (i.e. they must be in the same order).
 * 
 *******************************************************************************/ 
 
 /* This is the list of handlers for the xtra. the versionInfo string is combined
 /*	with the msgTable string in the register method to create a single string that
 /* used when registering the xtra as a scripting xtra. */

static char versionInfo[] = "xtra LeechProtectionRemovalHelp -- version %s.%s.%s\n";
static char msgTable[] = { 
	"new object me\n"  /* standard first handler entry in all message tables */
 	"-- Set Property Handlers --\n" 
	"* setTheMoviePath string moviePath -- sets the moviePath property\n"
	"* setTheRunMode string runMode -- sets the runMode property\n"
	"* setThePlatform string platform -- sets the platform property\n"
	"* setTheMachineType int machineType -- sets the machineType property\n"
	"* setTheEnvironment_ShockMachine int environment_ShockMachine -- sets the environment.shockMachine property\n"
 	"-- Force Property Handlers --\n" 
	"* forceTheExitLock int exitLock -- forces the exitLock property\n"
	"-- Set External Param Handlers --\n"
	"* setExternalParam string name, string value -- sets an External Param\n"
 	"-- Disable Handler Handlers --\n" 
	"* disableGoToNetMovie -- disables goToNetMovie\n"
	"* disableGoToNetPage -- disables goToNetPage\n"
 	"-- Bugfix Handlers --\n" 
	"* bugfixShockwave3DBadDriverList -- bugfixes Shockwave 3D Bad Driver List"
	};

enum 
{
	m_new = 0, /* standard first entry */
	m_setTheMoviePath,
	m_setTheRunMode,
	m_setThePlatform,
	m_setTheMachineType,
	m_setTheEnvironment_ShockMachine,
	m_forceTheExitLock,
	m_setExternalParam,
	m_disableGoToNetMovie,
	m_disableGoToNetPage,
	m_bugfixShockwave3DBadDriverList,
	m_XXXX /* standard last entry */
};




#define XTRA_VERSION_NUMBER XTRA_CLASS_VERSION

BEGIN_XTRA
	BEGIN_XTRA_DEFINES_CLASS(TStdXtra, XTRA_CLASS_VERSION)
		CLASS_DEFINES_INTERFACE(TStdXtra, IMoaRegister, XTRA_VERSION_NUMBER)
		CLASS_DEFINES_INTERFACE(TStdXtra, IMoaMmXScript, XTRA_VERSION_NUMBER)
	END_XTRA_DEFINES_CLASS
END_XTRA

STDMETHODIMP_(MoaError) MoaCreate_TStdXtra(TStdXtra * This) {
	moa_try
	
	ThrowErr(This->pCallback->QueryInterface(&IID_IMoaMmValue, (PPMoaVoid)&This->pMoaMmValueInterface));
	ThrowErr(This->pCallback->QueryInterface(&IID_IMoaMmUtils2, (PPMoaVoid)&This->pMoaMmUtilsInterface));
	ThrowErr(This->pCallback->QueryInterface(&IID_IMoaDrPlayer, (PPMoaVoid)&This->pMoaDrPlayerInterface));

	moa_catch
	moa_catch_end
	moa_try_end
}

STDMETHODIMP_(void) MoaDestroy_TStdXtra(TStdXtra * This) {
	moa_try

	if (This->pMoaMmValueInterface) {
		This->pMoaMmValueInterface->Release();
		This->pMoaMmValueInterface = 0;
	}

	if (This->pMoaMmUtilsInterface) {
		This->pMoaMmUtilsInterface->Release();
		This->pMoaMmUtilsInterface = 0;
	}

	if (This->pMoaDrPlayerInterface) {
		This->pMoaDrPlayerInterface->Release();
		This->pMoaDrPlayerInterface = 0;
	}

	moa_catch
	moa_catch_end
	moa_try_end_void
}




STD_INTERFACE_CREATE_DESTROY(TStdXtra, IMoaRegister)
BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaRegister)
END_DEFINE_CLASS_INTERFACE

STDMETHODIMP TStdXtra_IMoaRegister::Register(PIMoaCache pCache, PIMoaXtraEntryDict pXtraDict) {	
	moa_try
	PIMoaRegistryEntryDict		pReg;
	MoaBool						bItsSafe = TRUE;
	char versionStr[256];
	PMoaVoid pMemStr = NULL;

	/* Register the lingo xtra */
	ThrowErr(pCache->AddRegistryEntry(pXtraDict, &CLSID_TStdXtra, &IID_IMoaMmXScript, &pReg));

	/* Register the method table */
	const char *VER_MAJORVERSION_STRING = "1";
	const char *VER_MINORVERSION_STRING = "2";
	const char *VER_BUGFIXVERSION_STRING = "1";

	sprintf_s(versionStr, sizeof(versionStr), versionInfo, VER_MAJORVERSION_STRING, VER_MINORVERSION_STRING, VER_BUGFIXVERSION_STRING);

	pMemStr = pObj->pCalloc->NRAlloc(sizeof(versionStr) + sizeof(msgTable));

	ThrowNull(pMemStr);

	strcpy_s((char *)pMemStr, sizeof(versionStr), versionStr);
	strcat_s((char *)pMemStr, sizeof(versionStr) + sizeof(msgTable), msgTable);

	ThrowErr(pReg->Put(kMoaDrDictType_MessageTable, pMemStr, 0, kMoaDrDictKey_MessageTable));

	/* Mark xtra as safe for shockwave - but only if it IS safe ! */
	/*
	ThrowErr(pReg->Put(kMoaMmDictType_SafeForShockwave, &bItsSafe, sizeof(bItsSafe), kMoaMmDictKey_SafeForShockwave));
	*/

	moa_catch
	moa_catch_end
	if (pMemStr) {
		pObj->pCalloc->NRFree(pMemStr);
	}
	moa_try_end
}




BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaMmXScript)
END_DEFINE_CLASS_INTERFACE

TStdXtra_IMoaMmXScript::TStdXtra_IMoaMmXScript(MoaError * pError) {
	*pError = kMoaErr_NoErr;
}	

TStdXtra_IMoaMmXScript::~TStdXtra_IMoaMmXScript() {
}

STDMETHODIMP TStdXtra_IMoaMmXScript::Call(PMoaDrCallInfo callPtr) {
	moa_try
	// switch statement with all the handlers that may be called - we arrive here from Lingo first
	switch (callPtr->methodSelector) {
		case m_setTheMoviePath:
		ThrowErr(XScrpGeneral(callPtr, (PMoaChar)theMoviePath, MODULE_HANDLE_SET_DIRECTOR_API));
		break;
		case m_setTheRunMode:
		ThrowErr(XScrpGeneral(callPtr, (PMoaChar)theRunMode, MODULE_HANDLE_SET_DIRECTOR_API));
		break;
		case m_setThePlatform:
		ThrowErr(XScrpGeneral(callPtr, (PMoaChar)thePlatform, MODULE_HANDLE_SET_DIRECTOR_API));
		break;
		case m_setTheMachineType:
		ThrowErr(XScrpGeneral(callPtr, &theMachineType, MODULE_HANDLE_SET_DIRECTOR_API));
		break;
		case m_setTheEnvironment_ShockMachine:
		ThrowErr(XScrpGeneral(callPtr, &theEnvironment_ShockMachine, MODULE_HANDLE_SET_DIRECTOR_API));
		break;
		case m_forceTheExitLock:
		ThrowErr(XScrpGeneral(callPtr, &theExitLock, MODULE_HANDLE_SET_DIRECTOR_API));
		break;
		case m_setExternalParam:
		// this one requires a bit of fiddling with strings first
		ThrowErr(XScrpSetExternalParam(callPtr, MODULE_HANDLE_SET_DIRECTOR_API));
		break;
		case m_disableGoToNetMovie:
		case m_disableGoToNetPage:
		// these guys are in the NetLingo Xtra
		ThrowErr(XScrpGeneral(callPtr, MODULE_HANDLE_SET_NET_LINGO_XTRA));
		break;
		case m_bugfixShockwave3DBadDriverList:
		// this one is in the Shockwave 3D Asset Xtra
		ThrowErr(XScrpGeneral(callPtr, MODULE_HANDLE_SET_SHOCKWAVE_3D_ASSET_XTRA));
	}
	moa_catch
	moa_catch_end
	moa_try_end
}




MoaError getTextSectionAddressAndSize(LPVOID &textSectionLpAddress, SIZE_T &textSectionDwSize, HANDLE moduleHandle, PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, MoaMmSymbol alertSymbol) {
	moa_try

	MoaError err = kMoaErr_NoErr;
	MoaMmValue displayString = kVoidMoaMmValueInitializer;

	// just in case, we check again the Module Handle is valid
	if (moduleHandle == NULL) {
		ThrowErr(pMoaMmValueInterface->StringToValue("Failed to get Module Handle", &displayString));
		ThrowErr(pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL));
		ThrowErr(pMoaMmValueInterface->StringToValue("An Xtra that is required is missing. You may have called the bugfixShockwave3DBadDriverList handler but didn't include the Shockwave 3D Asset Xtra in your Projector.", &displayString));
		pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		return kMoaErr_YesErr;
	}
	// the point of this function is to find the address and size of the .text section so we can modify its code
	// the handy Win32 API ImageNSetTheader function will help sort us out here
	PIMAGE_NT_HEADERS imageNSetTheader = ImageNtHeader(moduleHandle);
	if (!imageNSetTheader) {
		err = pMoaMmValueInterface->StringToValue("Failed to get Image NT Header", &displayString);
		if (err != kMoaErr_NoErr) {
			imageNSetTheader = 0;
			return err;
		}
		pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		imageNSetTheader = 0;
		return kMoaErr_YesErr;
	}
	// right after that is what we need, the Image Section Header, which has the names of the sections
	PIMAGE_SECTION_HEADER imageSectionHeader = (PIMAGE_SECTION_HEADER)(imageNSetTheader + 1);
	// the name of the section we want, IMAGE_SIZEOF_SHORT_NAME is from Win32 API and helps us out here again
	const char IMAGE_SECTION_HEADER_NAME_TEXT[IMAGE_SIZEOF_SHORT_NAME] = ".text\0\0";
	// loop through all sections looking for .text section
	for (WORD i = 0;i<imageNSetTheader->FileHeader.NumberOfSections;i++) {
		// in case the module unloads during this loop (unlikely, but you can never be too careful I suppose)
		if (moduleHandle == NULL) {
			err = pMoaMmValueInterface->StringToValue("Failed to get Module Handle", &displayString);
			if (err != kMoaErr_NoErr) {
				imageNSetTheader = 0;
				imageSectionHeader = 0;
				return err;
			}
			err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
			if (err != kMoaErr_NoErr) {
				imageNSetTheader = 0;
				imageSectionHeader = 0;
				return err;
			}
			err = pMoaMmValueInterface->StringToValue("A required Xtra was unloaded while it was in use by the LeechProtectionRemovalHelp Xtra. Please do not unload Xtras while the LeechProtectionRemovalHelp Xtra is loaded.", &displayString);
			if (err != kMoaErr_NoErr) {
				imageNSetTheader = 0;
				imageSectionHeader = 0;
				return err;
			}
			pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
			imageNSetTheader = 0;
			imageSectionHeader = 0;
			return kMoaErr_YesErr;
		}
		// false means they are the same
		if (!memcmp((const void*)imageSectionHeader->Name, IMAGE_SECTION_HEADER_NAME_TEXT, IMAGE_SIZEOF_SHORT_NAME)) {
			textSectionLpAddress = (BYTE*)moduleHandle + imageSectionHeader->VirtualAddress;
			textSectionDwSize = imageSectionHeader->Misc.VirtualSize;
			imageNSetTheader = 0;
			imageSectionHeader = 0;
			return kMoaErr_NoErr;
		}
		imageSectionHeader++;
	}
	// if we're here, the loop finished and there was no section with a .text short name, so error out
	err = pMoaMmValueInterface->StringToValue("Failed to get Text Section", &displayString);
	if (err != kMoaErr_NoErr) {
		imageNSetTheader = 0;
		imageSectionHeader = 0;
		return err;
	}
	err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
	if (err != kMoaErr_NoErr) {
		imageNSetTheader = 0;
		imageSectionHeader = 0;
		return err;
	}
	err = pMoaMmValueInterface->StringToValue("A required Xtra is corrupted, garbled or tampered with. Please use the official Xtras from Adobe.", &displayString);
	if (err != kMoaErr_NoErr) {
		imageNSetTheader = 0;
		imageSectionHeader = 0;
		return err;
	}
	pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
	imageNSetTheader = 0;
	imageSectionHeader = 0;
	return kMoaErr_YesErr;

	moa_catch
	moa_catch_end
	moa_try_end
}

MoaError setupExtenderTestIt(TextSection* textSection,
						 unsigned char tested85[], unsigned char tested10[], unsigned char tested115[],
						 TESTED_SET &testedSet,
						 size_t sizeofTested85, size_t sizeofTested10, size_t sizeofTested115,
						 DWORD testedAddress85, DWORD testedAddress10, DWORD testedAddress115,
						 PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, MoaMmSymbol alertSymbol) {
	moa_try

	if (!testedSet) {
		MoaError err = kMoaErr_NoErr;
		MoaMmValue displayString = kVoidMoaMmValueInitializer;

		// performs a test to see if this is a supported Director version
		// it's a simple array of bytes search, nothing more
		// we support 8.5, 10 and 11.5, so we perform three tests
		if (!textSection->test(tested85, sizeofTested85, testedAddress85)) {
			if (!textSection->test(tested10, sizeofTested10, testedAddress10)) {
				if (!textSection->test(tested115, sizeofTested115, testedAddress115)) {
					err = pMoaMmValueInterface->StringToValue("Text Section Test failed", &displayString);
					if (err != kMoaErr_NoErr) {
						return err;
					}
					err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					if (err != kMoaErr_NoErr) {
						return err;
					}
					err = pMoaMmValueInterface->StringToValue("The LeechProtectionRemovalHelp Xtra is only compatible with Director 8.5.0.321, 10.0.0.188, and 11.5.0.593 as well as their associated Xtras.", &displayString);
					if (err != kMoaErr_NoErr) {
						return err;
					}
					pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					return kMoaErr_YesErr;
				} else {
					testedSet = TESTED_SET_115;
				}
			} else {
				testedSet = TESTED_SET_10;
			}
		} else {
			testedSet = TESTED_SET_85;
		}
	}
	return kMoaErr_NoErr;

	moa_catch
	moa_catch_end
	moa_try_end
}

MoaError setupExtender(MoaMmSymbol methodSelector, HANDLE currentProcess, MODULE_HANDLE_SET moduleHandleSet, PIMoaMmValue pMoaMmValueInterface, PIMoaDrMovie pMoaDrMovieInterface, MoaMmSymbol alertSymbol) {
	moa_try

	MoaError err = kMoaErr_NoErr;
	MoaMmValue displayString = kVoidMoaMmValueInitializer;

	HANDLE moduleHandle = NULL;

	// this is where the bulk of the code lies
	// much of it is repeated code with just slight enough variations to make it difficult to modularize
	switch (moduleHandleSet) {
		case MODULE_HANDLE_SET_DIRECTOR_API:
		if (!moduleHandleDirectorAPISet) {
			moduleHandleDirectorAPI = GetModuleHandle("DIRAPI");
			moduleHandleDirectorAPISet = true;
		}
		moduleHandle = moduleHandleDirectorAPI;
		break;
		case MODULE_HANDLE_SET_NET_LINGO_XTRA:
		if (!moduleHandleNetLingoXtraSet) {
			moduleHandleNetLingoXtra = GetModuleHandle("NetLingo.x32");
			moduleHandleNetLingoXtraSet = true;
		}
		moduleHandle = moduleHandleNetLingoXtra;
		break;
		case MODULE_HANDLE_SET_SHOCKWAVE_3D_ASSET_XTRA:
		if (!moduleHandleShockwave3DAssetXtraSet) {
			moduleHandleShockwave3DAssetXtra = GetModuleHandle("Shockwave 3D Asset.x32");
			moduleHandleShockwave3DAssetXtraSet = true;
		}
		moduleHandle = moduleHandleShockwave3DAssetXtra;
		break;
		default:
		// this should be impossible
		ThrowErr(pMoaMmValueInterface->StringToValue("Null Module Handle Set", &displayString));
		pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		return kMoaErr_YesErr;
	}

	// here is the first check that the Module Handle is valid
	if (moduleHandle == NULL) {
		ThrowErr(pMoaMmValueInterface->StringToValue("Failed to get Module Handle", &displayString));
		ThrowErr(pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL));
		ThrowErr(pMoaMmValueInterface->StringToValue("An Xtra that is required is missing. You may have called the bugfixShockwave3DBadDriverList handler but didn't include the Shockwave 3D Asset Xtra in your Projector.", &displayString));
		pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		return kMoaErr_YesErr;
	}

	LPVOID textSectionLpAddress = NULL;
	SIZE_T textSectionDwSize = 0;

	// get Text Section lpAddress and dwSize
	err = getTextSectionAddressAndSize(textSectionLpAddress, textSectionDwSize, moduleHandle, pMoaMmValueInterface, pMoaDrMovieInterface, alertSymbol);
	if (err != kMoaErr_NoErr) {
		ThrowErr(pMoaMmValueInterface->StringToValue("Failed to get Text Section Address and Size", &displayString));
		pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		return err;
	}

	// create Text Section
	TextSection* textSection = new TextSection(textSectionLpAddress, textSectionDwSize, currentProcess);
	if (!textSection) {
		ThrowErr(pMoaMmValueInterface->StringToValue("Failed to create Text Section", &displayString));
		pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		return kMoaErr_YesErr;
	}
	// if we don't unprotect the Text Section, we'll get an access violation in attempting to edit it
	if (!textSection->unprotect()) {
		err = pMoaMmValueInterface->StringToValue("Failed to unprotect Text Section", &displayString);
		if (err != kMoaErr_NoErr) {
			delete textSection;
			textSection = 0;
			return err;
		}
		err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		if (err != kMoaErr_NoErr) {
			delete textSection;
			textSection = 0;
			return err;
		}
		err = pMoaMmValueInterface->StringToValue("Your antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.", &displayString);
		if (err != kMoaErr_NoErr) {
			delete textSection;
			textSection = 0;
			return err;
		}
		pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		delete textSection;
		textSection = 0;
		return kMoaErr_YesErr;
	}

	// this variable is true when we've successfully written everything required
	// the procedure is mostly the same for all modules
	bool writeSet = false;
	switch (moduleHandleSet) {
		case MODULE_HANDLE_SET_DIRECTOR_API:

		// test it
		{
			const size_t SIZEOF_TESTED_DIRECTOR_API_85 = 5;
			const size_t SIZEOF_TESTED_DIRECTOR_API_10 = 5;
			const size_t SIZEOF_TESTED_DIRECTOR_API_115 = 7;
			unsigned char testedDirectorAPI85[SIZEOF_TESTED_DIRECTOR_API_85] = {0xE8, 0xE1, 0x3A, 0xF6, 0xFF};
			unsigned char testedDirectorAPI10[SIZEOF_TESTED_DIRECTOR_API_10] = {0xE8, 0x73, 0xE9, 0xF5, 0xFF};
			unsigned char testedDirectorAPI115[SIZEOF_TESTED_DIRECTOR_API_115] = {0x8B, 0x84, 0x24, 0x30, 0x04, 0x00, 0x00};
			err = setupExtenderTestIt(textSection,
									  testedDirectorAPI85, testedDirectorAPI10, testedDirectorAPI115,
									  testedDirectorAPISet,
									  SIZEOF_TESTED_DIRECTOR_API_85, SIZEOF_TESTED_DIRECTOR_API_10, SIZEOF_TESTED_DIRECTOR_API_115,
									  0x000A8592, 0x000AD97D, 0x000DEE85,
									  pMoaMmValueInterface, pMoaDrMovieInterface, alertSymbol);
			if (err != kMoaErr_NoErr) {
				delete textSection;
				textSection = 0;
				return err;
			}
		}

		// write it
		switch (testedDirectorAPISet) {
			case TESTED_SET_85:
			// populate the Module Handle Written Code Return Addresses, now that they're relevant
			// here is where the real writes happen at the appropriate addresses
			switch (methodSelector) {
				case m_setTheMoviePath:
				moduleHandleWrittenCodeReturnAddressSetTheMoviePath = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_MOVIE_PATH_85);
				writeSet = textSection->write(writtenSetTheMoviePath85, WRITTEN_ADDRESS_SET_THE_MOVIE_PATH_85);
				break;
				case m_setTheRunMode:
				moduleHandleWrittenCodeReturnAddressSetTheRunMode = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_RUN_MODE_85);
				writeSet = textSection->write(writtenSetTheRunMode85, WRITTEN_ADDRESS_SET_THE_RUN_MODE_85);
				break;
				case m_setThePlatform:
				moduleHandleWrittenCodeReturnAddressSetThePlatform = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_PLATFORM_85);
				writeSet = textSection->write(writtenSetThePlatform85, WRITTEN_ADDRESS_SET_THE_PLATFORM_85);
				break;
				case m_setTheMachineType:
				writeSet = textSection->write(writtenSetTheMachineType85, WRITTEN_ADDRESS_SET_THE_MACHINE_TYPE_85);
				break;
				case m_setTheEnvironment_ShockMachine:
				moduleHandleWrittenCodeReturnAddressSetTheEnvironment_ShockMachine = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_85);
				writeSet = textSection->write(writtenSetTheEnvironment_ShockMachine85, WRITTEN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_85);
				break;
				case m_forceTheExitLock:
				moduleHandleWrittenCodeReturnAddressForceTheExitLock = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_FORCE_THE_EXIT_LOCK_85);
				if (!textSection->write(writtenForceTheExitLock, WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_85) || !textSection->write(writtenForceTheExitLock23, WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_2_85)) {
					err = pMoaMmValueInterface->StringToValue("Text Section Write failed", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaMmValueInterface->StringToValue("Your antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					delete textSection;
					textSection = 0;
					return kMoaErr_YesErr;
				}
				writeSet = textSection->write(writtenForceTheExitLock23, WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_3_85);
				break;
				case m_setExternalParam:
				moduleHandleWrittenCodeReturnAddressSetExternalParamName = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_NAME_85);
				moduleHandleWrittenCodeReturnAddressSetExternalParamValue = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_85);
				moduleHandleWrittenCodeReturnAddressSetExternalParamCount = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_85);
				// here, we NOP out six instructions in a row that would cause our code to go awry
				for (DWORD i = 0;i < 6;i++) {
					textSection->write(WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_85_2 + i);
				}
				// this particular function is the same in 8.5 and 10 but not 11.5, hence the name
				if (!textSection->write(writtenSetExternalParamName8510, WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_85) || !textSection->write(writtenSetExternalParamValue8510, WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_85)) {
					err = pMoaMmValueInterface->StringToValue("Text Section Write failed", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaMmValueInterface->StringToValue("Your antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					delete textSection;
					textSection = 0;
					return kMoaErr_YesErr;
				}
				// we have to update the count of External Params too
				writeSet = textSection->write(writtenSetExternalParamCount8510, WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_85);
			}
			break;
			case TESTED_SET_10:
			// this code is much the same as for 8.5
			// just with version 10 instead
			// and a few small differences
			// fun right?
			switch (methodSelector) {
				case m_setTheMoviePath:
				moduleHandleWrittenCodeReturnAddressSetTheMoviePath = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_MOVIE_PATH_10);
				writeSet = textSection->write(writtenSetTheMoviePath10, WRITTEN_ADDRESS_SET_THE_MOVIE_PATH_10);
				break;
				case m_setTheRunMode:
				moduleHandleWrittenCodeReturnAddressSetTheRunMode = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_RUN_MODE_10);
				writeSet = textSection->write(writtenSetTheRunMode10, WRITTEN_ADDRESS_SET_THE_RUN_MODE_10);
				break;
				case m_setThePlatform:
				moduleHandleWrittenCodeReturnAddressSetThePlatform = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_PLATFORM_10);
				writeSet = textSection->write(writtenSetThePlatform10, WRITTEN_ADDRESS_SET_THE_PLATFORM_10);
				break;
				case m_setTheMachineType:
				writeSet = textSection->write(writtenSetTheMachineType10, WRITTEN_ADDRESS_SET_THE_MACHINE_TYPE_10);
				break;
				case m_setTheEnvironment_ShockMachine:
				moduleHandleWrittenCodeReturnAddressSetTheEnvironment_ShockMachine = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_10);
				writeSet = textSection->write(writtenSetTheEnvironment_ShockMachine10, WRITTEN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_10);
				break;
				case m_forceTheExitLock:
				moduleHandleWrittenCodeReturnAddressForceTheExitLock = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_FORCE_THE_EXIT_LOCK_10);
				// the reason for the if statement in addition to the writeSet variable here is there are two conditions that could go wrong
				// we need to exit as soon as anything goes wrong if possible
				if (!textSection->write(writtenForceTheExitLock, WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_10) || !textSection->write(writtenForceTheExitLock23, WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_2_10)) {
					err = pMoaMmValueInterface->StringToValue("Text Section Write failed", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaMmValueInterface->StringToValue("Your antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					delete textSection;
					textSection = 0;
					return kMoaErr_YesErr;
				}
				writeSet = textSection->write(writtenForceTheExitLock23, WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_3_10);
				break;
				case m_setExternalParam:
				moduleHandleWrittenCodeReturnAddressSetExternalParamName = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_NAME_10);
				moduleHandleWrittenCodeReturnAddressSetExternalParamValue = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_10);
				moduleHandleWrittenCodeReturnAddressSetExternalParamCount = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_10);
				for (DWORD i = 0;i < 6;i++) {
					textSection->write(WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_10_2 + i);
				}
				if (!textSection->write(writtenSetExternalParamName8510, WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_10) || !textSection->write(writtenSetExternalParamValue8510, WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_10)) {
					err = pMoaMmValueInterface->StringToValue("Text Section Write failed", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaMmValueInterface->StringToValue("Your antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					delete textSection;
					textSection = 0;
					return kMoaErr_YesErr;
				}
				writeSet = textSection->write(writtenSetExternalParamCount8510, WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_10);
			}
			break;
			case TESTED_SET_115:
			switch (methodSelector) {
				case m_setTheMoviePath:
				moduleHandleWrittenCodeReturnAddressSetTheMoviePath = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_MOVIE_PATH_115);
				writeSet = textSection->write(writtenSetTheMoviePath115, WRITTEN_ADDRESS_SET_THE_MOVIE_PATH_115);
				break;
				case m_setTheRunMode:
				moduleHandleWrittenCodeReturnAddressSetTheRunMode = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_RUN_MODE_115);
				moduleHandleWrittenCodeReturnAddressSetTheRunMode2 = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_RUN_MODE_115_2);
				if (!textSection->write(writtenSetTheRunMode115, WRITTEN_ADDRESS_SET_THE_RUN_MODE_115)) {
					err = pMoaMmValueInterface->StringToValue("Text Section Write failed", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaMmValueInterface->StringToValue("Your antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					delete textSection;
					textSection = 0;
					return kMoaErr_YesErr;
				}
				writeSet = textSection->write(writtenSetTheRunMode1152, WRITTEN_ADDRESS_SET_THE_RUN_MODE_115_2);
				break;
				case m_setThePlatform:
				moduleHandleWrittenCodeReturnAddressSetThePlatform = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_PLATFORM_115);
				writeSet = textSection->write(writtenSetThePlatform115, WRITTEN_ADDRESS_SET_THE_PLATFORM_115);
				break;
				case m_setTheMachineType:
				moduleHandleWrittenCodeReturnAddressSetTheEnvironment_ShockMachine = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_115);
				writeSet = textSection->write(writtenSetTheMachineType115, WRITTEN_ADDRESS_SET_THE_MACHINE_TYPE_115);
				break;
				case m_setTheEnvironment_ShockMachine:
				writeSet = textSection->write(writtenSetTheEnvironment_ShockMachine115, WRITTEN_ADDRESS_SET_THE_ENVIRONMENT_SHOCK_MACHINE_115);
				break;
				case m_forceTheExitLock:
				moduleHandleWrittenCodeReturnAddressForceTheExitLock = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_FORCE_THE_EXIT_LOCK_115);
				if (!textSection->write(writtenForceTheExitLock, WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_115) || !textSection->write(writtenForceTheExitLock23, WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_2_115)) {
					err = pMoaMmValueInterface->StringToValue("Text Section Write failed", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaMmValueInterface->StringToValue("Your antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					delete textSection;
					textSection = 0;
					return kMoaErr_YesErr;
				}
				writeSet = textSection->write(writtenForceTheExitLock23, WRITTEN_ADDRESS_FORCE_THE_EXIT_LOCK_3_115);
				break;
				case m_setExternalParam:
				moduleHandleWrittenCodeReturnAddressSetExternalParamName = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_NAME_115);
				moduleHandleWrittenCodeReturnAddressSetExternalParamValue = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_115);
				moduleHandleWrittenCodeReturnAddressSetExternalParamCount = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_115);
				for (DWORD i = 0;i < 6;i++) {
					textSection->write(WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_115_2 + i);
				}
				if (!textSection->write(writtenSetExternalParamName115, WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_NAME_115) || !textSection->write(writtenSetExternalParamValue115, WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_VALUE_115)) {
					err = pMoaMmValueInterface->StringToValue("Text Section Write failed", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					err = pMoaMmValueInterface->StringToValue("Your antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.", &displayString);
					if (err != kMoaErr_NoErr) {
						delete textSection;
						textSection = 0;
						return err;
					}
					pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
					delete textSection;
					textSection = 0;
					return kMoaErr_YesErr;
				}
				writeSet = textSection->write(writtenSetExternalParamCount115, WRITTEN_ADDRESS_SET_EXTERNAL_PARAM_COUNT_115);
			}
		}
		break;
		case MODULE_HANDLE_SET_NET_LINGO_XTRA:

		// much simpler than the Director API, there are only two very similar things you can do here
		// test it
		{
			const size_t SIZEOF_TESTED_NET_LINGO_XTRA_85 = 5;
			const size_t SIZEOF_TESTED_NET_LINGO_XTRA_10 = 5;
			const size_t SIZEOF_TESTED_NET_LINGO_XTRA_115 = 5;
			unsigned char testedNetLingoXtra85[SIZEOF_TESTED_NET_LINGO_XTRA_85] = {0xE8, 0x8F, 0x2D, 0x00, 0x00};
			unsigned char testedNetLingoXtra10[SIZEOF_TESTED_NET_LINGO_XTRA_10] = {0xE8, 0xB2, 0x2F, 0x00, 0x00};
			unsigned char testedNetLingoXtra115[SIZEOF_TESTED_NET_LINGO_XTRA_115] = {0xE8, 0x92, 0x56, 0x00, 0x00};
			err = setupExtenderTestIt(textSection,
									  testedNetLingoXtra85, testedNetLingoXtra10, testedNetLingoXtra115,
									  testedNetLingoXtraSet,
									  SIZEOF_TESTED_NET_LINGO_XTRA_85, SIZEOF_TESTED_NET_LINGO_XTRA_10, SIZEOF_TESTED_NET_LINGO_XTRA_115,
									  0x00001AB2, 0x00001AB5, 0x00001979,
									  pMoaMmValueInterface, pMoaDrMovieInterface, alertSymbol);
			if (err != kMoaErr_NoErr) {
				delete textSection;
				textSection = 0;
				return err;
			}
		}

		// blanking them out is the same - only the address differs
		// write it
		switch (testedNetLingoXtraSet) {
			case TESTED_SET_85:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				writeSet = textSection->write(writtenDisableGoToNetThing, WRITTEN_ADDRESS_DISABLE_GO_TO_NET_MOVIE_85);
				break;
				case m_disableGoToNetPage:
				writeSet = textSection->write(writtenDisableGoToNetThing, WRITTEN_ADDRESS_DISABLE_GO_TO_NET_PAGE_85);
			}
			break;
			case TESTED_SET_10:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				writeSet = textSection->write(writtenDisableGoToNetThing, WRITTEN_ADDRESS_DISABLE_GO_TO_NET_MOVIE_10);
				break;
				case m_disableGoToNetPage:
				writeSet = textSection->write(writtenDisableGoToNetThing, WRITTEN_ADDRESS_DISABLE_GO_TO_NET_PAGE_10);
			}
			break;
			case TESTED_SET_115:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				writeSet = textSection->write(writtenDisableGoToNetThing, WRITTEN_ADDRESS_DISABLE_GO_TO_NET_MOVIE_115);
				break;
				case m_disableGoToNetPage:
				writeSet = textSection->write(writtenDisableGoToNetThing, WRITTEN_ADDRESS_DISABLE_GO_TO_NET_PAGE_115);
			}
		}
		break;
		case MODULE_HANDLE_SET_SHOCKWAVE_3D_ASSET_XTRA:

		// test it
		{
			const size_t SIZEOF_TESTED_SHOCKWAVE_3D_ASSET_XTRA_85 = 9;
			const size_t SIZEOF_TESTED_SHOCKWAVE_3D_ASSET_XTRA_10 = 9;
			const size_t SIZEOF_TESTED_SHOCKWAVE_3D_ASSET_XTRA_115 = 9;
			unsigned char testedShockwave3DAssetXtra85[SIZEOF_TESTED_SHOCKWAVE_3D_ASSET_XTRA_85] = {0xFF, 0x75, 0xE4, 0xFF, 0x15, 0x08, 0xB0, 0x24, 0x7A};
			unsigned char testedShockwave3DAssetXtra10[SIZEOF_TESTED_SHOCKWAVE_3D_ASSET_XTRA_10] = {0xFF, 0x75, 0xF4, 0xFF, 0x15, 0x0C, 0xA0, 0x26, 0x7A};
			unsigned char testedShockwave3DAssetXtra115[SIZEOF_TESTED_SHOCKWAVE_3D_ASSET_XTRA_115] = {0xFF, 0x75, 0xE4, 0xFF, 0x15, 0x0C, 0x10, 0x26, 0x7A};
			err = setupExtenderTestIt(textSection,
									  testedShockwave3DAssetXtra85, testedShockwave3DAssetXtra10, testedShockwave3DAssetXtra115,
									  testedShockwave3DAssetXtraSet,
									  SIZEOF_TESTED_SHOCKWAVE_3D_ASSET_XTRA_85, SIZEOF_TESTED_SHOCKWAVE_3D_ASSET_XTRA_10, SIZEOF_TESTED_SHOCKWAVE_3D_ASSET_XTRA_115,
									  0x000DC45E, 0x000F515C, 0x000D6AEF,
									  pMoaMmValueInterface, pMoaDrMovieInterface, alertSymbol);
			if (err != kMoaErr_NoErr) {
				delete textSection;
				textSection = 0;
				return err;
			}
		}

		// write it
		switch (testedShockwave3DAssetXtraSet) {
			case TESTED_SET_85:
			// only one case currently, but open for future expansion
			moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_85);
			moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList2 = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_85_2);
			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				writeSet = textSection->write(writtenBugfixShockwave3DBadDriverList85, WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_85);
				writeSet = textSection->write(writtenBugfixShockwave3DBadDriverList852, WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_85_2);
			}
			break;
			case TESTED_SET_10:
			moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_10);
			moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList2 = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_10_2);
			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				writeSet = textSection->write(writtenBugfixShockwave3DBadDriverList10, WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_10);
				writeSet = textSection->write(writtenBugfixShockwave3DBadDriverList102, WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_10_2);
			}
			break;
			case TESTED_SET_115:
			moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_115);
			moduleHandleWrittenCodeReturnAddressBugfixShockwave3DBadDriverList2 = (HANDLE)((DWORD)moduleHandle + (DWORD)WRITTEN_CODE_RETURN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_115_2);
			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				writeSet = textSection->write(writtenBugfixShockwave3DBadDriverList115, WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_115);
				writeSet = textSection->write(writtenBugfixShockwave3DBadDriverList1152, WRITTEN_ADDRESS_BUGFIX_SHOCKWAVE_3D_BAD_DRIVER_LIST_115_2);
			}
		}
	}

	// in case we failed and didn't catch it somehow
	if (!writeSet) {
		err = pMoaMmValueInterface->StringToValue("Text Section Write failed", &displayString);
		if (err != kMoaErr_NoErr) {
			delete textSection;
			textSection = 0;
			return err;
		}
		err = pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		if (err != kMoaErr_NoErr) {
			delete textSection;
			textSection = 0;
			return err;
		}
		err = pMoaMmValueInterface->StringToValue("Your antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.", &displayString);
		if (err != kMoaErr_NoErr) {
			delete textSection;
			textSection = 0;
			return err;
		}
		pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		delete textSection;
		textSection = 0;
		return kMoaErr_YesErr;
	}

	// this just makes our writes actually take effect, naturally
	// flush it
	if (!textSection->flush()) {
		err = pMoaMmValueInterface->StringToValue("Text Section Flush failed", &displayString);
		if (err != kMoaErr_NoErr) {
			delete textSection;
			textSection = 0;
			return err;
		}
		pMoaDrMovieInterface->CallHandler(alertSymbol, 1, &displayString, NULL);
		delete textSection;
		textSection = 0;
		return kMoaErr_YesErr;
	}

	// cleanup
	delete textSection;
	textSection = 0;
	return kMoaErr_NoErr;

	moa_catch
	moa_catch_end
	moa_try_end
}

MoaError TStdXtra_IMoaMmXScript::XScrpGeneral(PMoaDrCallInfo callPtr, MODULE_HANDLE_SET moduleHandleSet) {
	moa_try

	MoaError err = kMoaErr_NoErr;

	// the current process (both to create the Text Section and to terminate it if need be)
	HANDLE currentProcess = GetCurrentProcess();
	// we can't just use Win32 API's MessageBox or it'll cause graphical glitches from the movie not being paused
	// so we use Lingo's alert handler (as per the XDK's recommendation)
	MoaMmSymbol alertSymbol;
	ThrowErr(pObj->pMoaMmValueInterface->StringToSymbol("alert", &alertSymbol));
	// get the Active Movie (so we can run that handler in it if we need to)
	PIMoaDrMovie pMoaDrMovieInterface;
	ThrowErr(pObj->pMoaDrPlayerInterface->GetActiveMovie(&pMoaDrMovieInterface));
	err = setupExtender(callPtr->methodSelector, currentProcess, moduleHandleSet, pObj->pMoaMmValueInterface, pMoaDrMovieInterface, alertSymbol);
	if (err != kMoaErr_NoErr) {
		TerminateProcess(currentProcess, err);
		return err;
	}

	// don't forget to brush your teeth
	if (pMoaDrMovieInterface) {
		pMoaDrMovieInterface->Release();
		pMoaDrMovieInterface = 0;
	}

	return kMoaErr_NoErr;

	moa_catch
	moa_catch_end
	moa_try_end
}

MoaError TStdXtra_IMoaMmXScript::XScrpGeneral(PMoaDrCallInfo callPtr, PMoaLong property, MODULE_HANDLE_SET moduleHandleSet) {
	moa_try

	MoaMmValue argumentValue = kVoidMoaMmValueInitializer;

	// this is an overload for the same function as the previous but with an integers as an argument
	// we hang on to this property as a global variable
	// since it's just so our extension can access it
	AccessArgByIndex(1, &argumentValue);
	pObj->pMoaMmValueInterface->ValueToInteger(&argumentValue, property);

	// the rest is the same
	ThrowErr(XScrpGeneral(callPtr, moduleHandleSet));

	moa_catch
	moa_catch_end
	moa_try_end
}

MoaError TStdXtra_IMoaMmXScript::XScrpGeneral(PMoaDrCallInfo callPtr, PMoaChar property, MODULE_HANDLE_SET moduleHandleSet) {
	moa_try

	MoaMmValue argumentValue = kVoidMoaMmValueInitializer;

	// same deal again, this time for strings
	// Lingo expects the first byte to be a length (up to 255 naturally)
	AccessArgByIndex(1, &argumentValue);
	pObj->pMoaMmValueInterface->ValueToString(&argumentValue, (PMoaChar)(property + 1), 255);
	*(unsigned char*)property = (unsigned char)strlen((PMoaChar)(property + 1));

	ThrowErr(XScrpGeneral(callPtr, moduleHandleSet));

	moa_catch
	moa_catch_end
	moa_try_end
}

MoaError TStdXtra_IMoaMmXScript::XScrpSetExternalParam(PMoaDrCallInfo callPtr, MODULE_HANDLE_SET moduleHandleSet) {
	moa_try

	MoaMmValue argumentValue = kVoidMoaMmValueInitializer;

	AccessArgByIndex(1, &argumentValue);
	MoaChar name[256];
	pObj->pMoaMmValueInterface->ValueToString(&argumentValue, (PMoaChar)name, 255);

	AccessArgByIndex(2, &argumentValue);
	MoaChar value[256];
	pObj->pMoaMmValueInterface->ValueToString(&argumentValue, (PMoaChar)value, 255);

	PMoaChar externalParamsOld = externalParams;
	int sizeofExternalParamsOld = sizeofExternalParams;

	// adds the param tag to the "browser"
	if (sizeofExternalParams) {
		sizeofExternalParams = strlen(name) + 1 + strlen(value) + 1 + sizeofExternalParams;
		externalParams = new char[sizeofExternalParams];
		memset(externalParams, 0, sizeofExternalParams);
		memcpy_s(externalParams, sizeofExternalParams, name, strlen(name));
		memcpy_s(externalParams + strlen(name) + 1, sizeofExternalParams, value, strlen(value));
		memcpy_s(externalParams + strlen(name) + 1 + strlen(value) + 1, sizeofExternalParams, externalParamsOld, sizeofExternalParamsOld);
	} else {
		sizeofExternalParams = strlen(name) + 1 + strlen(value) + 3;
		externalParams = new char[sizeofExternalParams];
		memset(externalParams, 0, sizeofExternalParams);
		memcpy_s(externalParams, sizeofExternalParams, name, strlen(name));
		memcpy_s(externalParams + strlen(name) + 1, sizeofExternalParams, value, strlen(value));
	}

	delete[] externalParamsOld;
	externalParamsOld = 0;

	ThrowErr(XScrpGeneral(callPtr, moduleHandleSet));

	moa_catch
	moa_catch_end
	moa_try_end
}

// "For with God nothing will be impossible." - Luke 1:37