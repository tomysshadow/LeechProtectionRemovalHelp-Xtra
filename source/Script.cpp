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

#define INITGUID 1

#include "shared.h"
#include "script.h"
#include "Extender.h"

#pragma function(strlen)
#include "moastdif.h"
#include "mmixscrp.h"
#include "driservc.h"
#include "xmmvalue.h"
#include "xclassver.h"
#include "moatry.h"

#include <windows.h>




/* Begin Xtra */
/*******************************************************************************
* SCRIPTING XTRA MESSAGE TABLE DESCRIPTION.
*
* the general format is:
* xtra <nameOfXtra>
* new object me [ args ... ]
* <otherHandlerDefintions>
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

/* This is the list of handlers for the Xtra. The versionInfo string is combined
/*	with the msgTable string in the Register method to create a single string that
/* is used when registering the Xtra as a Scripting Xtra. */

static char versionInfo[] = "xtra LeechProtectionRemovalHelp -- version %s.%s.%s\n";
static char msgTable[] = {
	"new object me\n" /* standard first handler entry in all message tables */
	"-- Set Properties --\n"
	"* setTheMoviePath string moviePath -- sets the moviePath property\n"
	"-- and the path, and the pathName, and _movie.path\n"
	"* setTheMovieName string movieName -- sets the movieName property\n"
	"-- and the movie, and _movie.name\n"
	"* setTheEnvironment_shockMachine integer environment_shockMachine -- sets the environment.shockMachine property\n"
	"-- and the environmentPropList.shockMachine, and _system.environmentPropList.shockMachine\n"
	"* setTheEnvironment_shockMachineVersion string environment_shockMachineVersion -- sets the environment.shockMachineVersion property\n"
	"-- and the environmentPropList.shockMachineVersion, and _system.environmentPropList.shockMachineVersion\n"
	"* setThePlatform string platform -- sets the platform property\n"
	"-- and the environment.platform, and the environmentPropList.platform, and _system.environmentPropList.platform\n"
	"* setTheRunMode string runMode -- sets the runMode property\n"
	"-- and the environment.runMode, and the environmentPropList.runMode, and _system.environmentPropList.runMode, and _player.runMode\n"
	"* setTheEnvironment_productBuildVersion string environment_productBuildVersion -- sets the environment.productBuildVersion property\n"
	"-- and the environmentPropList.productBuildVersion, and _system.environmentPropList.productBuildVersion\n"
	"* setTheProductVersion string productVersion -- sets the productVersion property\n"
	"-- and the environment.productVersion, and the environmentPropList.productVersion, and _system.environmentPropList.productVersion, and _player.productVersion\n"
	"* setTheEnvironment_osVersion string environment_osVersion -- sets the environment.osVersion property\n"
	"-- and the environmentPropList.osVersion, and _system.environmentPropList.osVersion\n"
	"* setTheMachineType integer machineType -- sets the machineType property\n"
	"-- Set External Params --\n"
	"* setExternalParam string name, string value -- sets an External Param\n"
	"-- name must not be empty\n"
	"-- Force Properties --\n"
	"* forceTheExitLock integer exitLock -- forces the exitLock property\n"
	"* forceTheSafePlayer integer safePlayer -- forces the safePlayer property\n"
	"-- Disable Handlers --\n"
	"* disableGoToNetMovie -- disables goToNetMovie handler\n"
	"* disableGoToNetPage -- disables goToNetPage handler\n"
	"-- Bugfixes --\n"
	"* bugfixShockwave3DBadDriverList -- bugfixes Shockwave 3D Bad Driver List"
};

enum {
	m_new = 0, /* standard first entry */
	m_setTheMoviePath,
	m_setTheMovieName,
	m_setTheEnvironment_shockMachine,
	m_setTheEnvironment_shockMachineVersion,
	m_setThePlatform,
	m_setTheRunMode,
	m_setTheEnvironment_productBuildVersion,
	m_setTheProductVersion,
	m_setTheEnvironment_osVersion,
	m_setTheMachineType,
	m_setExternalParam,
	m_forceTheExitLock,
	m_forceTheSafePlayer,
	m_disableGoToNetMovie,
	m_disableGoToNetPage,
	m_bugfixShockwave3DBadDriverList,
	m_XXXX /* standard last entry */
};




// this is where the structure of the Xtra is defined
// this says that this is a Standard Xtra and
// that said Standard Xtra class provides the
// interfaces for IMoaRegister and IMoaMmXScript, as well as
// says which version of the XDK we're using
#define XTRA_VERSION_NUMBER XTRA_CLASS_VERSION

BEGIN_XTRA
BEGIN_XTRA_DEFINES_CLASS(TStdXtra, XTRA_CLASS_VERSION)
CLASS_DEFINES_INTERFACE(TStdXtra, IMoaRegister, XTRA_VERSION_NUMBER)
CLASS_DEFINES_INTERFACE(TStdXtra, IMoaMmXScript, XTRA_VERSION_NUMBER)
END_XTRA_DEFINES_CLASS
END_XTRA

// when the Standard Xtra is Created, we Query for Interfaces which
// are provided by Director - similar to how Director will
// query this Standard Xtra for the IMoaRegister and IMoaMmXScript interfaces
// that it provides to Director for its use
STDMETHODIMP_(MoaError) MoaCreate_TStdXtra(TStdXtra* This) {
	moa_try

	ThrowNull(This);

	ThrowErr(This->pCallback->QueryInterface(&IID_IMoaDrPlayer, (PPMoaVoid)&This->moaDrPlayerInterfacePointer));
	ThrowNull(This->moaDrPlayerInterfacePointer);

	ThrowErr(This->pCallback->QueryInterface(&IID_IMoaMmValue, (PPMoaVoid)&This->moaMmValueInterfacePointer));
	ThrowNull(This->moaMmValueInterfacePointer);

	// never unload this module
	HMODULE moduleHandle = NULL;

	if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCSTR)&extender, &moduleHandle)) {
		showLastError("Failed to Get Module Handle");
		terminateCurrentProcess();
		Throw(kMoaErr_OutOfMem);
	}

	moa_catch
	moa_catch_end
	moa_try_end
}

// when the Standard Xtra is Destroyed, it'll Release the
// interfaces we Queried for from Director before
STDMETHODIMP_(void) MoaDestroy_TStdXtra(TStdXtra* This) {
	moa_try

	ThrowNull(This);

	if (This->moaDrPlayerInterfacePointer) {
		This->moaDrPlayerInterfacePointer->Release();
		This->moaDrPlayerInterfacePointer = NULL;
	}

	if (This->moaMmValueInterfacePointer) {
		This->moaMmValueInterfacePointer->Release();
		This->moaMmValueInterfacePointer = NULL;
	}

	moa_catch
	moa_catch_end
	// we use moa_try_end_void instead of moa_try_end here since this method returns a void type
	moa_try_end_void
}




// this says that the Constructor/Deconstructor for
// the IMoaRegister interface provided by
// the Standard Xtra class will not do anything
STD_INTERFACE_CREATE_DESTROY(TStdXtra, IMoaRegister)

// this allows Director to
// Query and Release the IMoaRegister interface provided
// by the Standard Xtra class
BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaRegister)
END_DEFINE_CLASS_INTERFACE

STDMETHODIMP TStdXtra_IMoaRegister::Register(PIMoaCache cacheInterfacePointer, PIMoaXtraEntryDict xtraEntryDictInterfacePointer) {
	PMoaVoid memoryStringPointer = NULL;

	moa_try

	ThrowNull(cacheInterfacePointer);
	ThrowNull(xtraEntryDictInterfacePointer);

	{
		// this interface should NOT be released
		PIMoaRegistryEntryDict registryEntryDictInterfacePointer = NULL;
		ThrowErr(cacheInterfacePointer->AddRegistryEntry(xtraEntryDictInterfacePointer, &CLSID_TStdXtra, &IID_IMoaMmXScript, &registryEntryDictInterfacePointer));
		ThrowNull(registryEntryDictInterfacePointer);

		const char* VER_MAJORVERSION_STRING = "1";
		const char* VER_MINORVERSION_STRING = "6";
		const char* VER_BUGFIXVERSION_STRING = "0";

		const size_t VERSION_STRING_SIZE = min(256, kMoaMmMaxXtraMessageTable);
		char versionString[VERSION_STRING_SIZE] = "";

		if (sprintf_s(versionString, VERSION_STRING_SIZE, versionInfo, VER_MAJORVERSION_STRING, VER_MINORVERSION_STRING, VER_BUGFIXVERSION_STRING) == -1) {
			Throw(kMoaErr_OutOfMem);
		}

		MoaUlong versionStringSize = stringSizeMax(versionString, VERSION_STRING_SIZE);
		MoaUlong memoryStringSize = versionStringSize + strnlen_s(msgTable, kMoaMmMaxXtraMessageTable - versionStringSize);

		memoryStringPointer = pObj->pCalloc->NRAlloc(memoryStringSize);
		ThrowNull(memoryStringPointer);

		if (strncpy_s((char*)memoryStringPointer, memoryStringSize, versionString, versionStringSize)) {
			Throw(kMoaErr_OutOfMem);
		}

		if (strcat_s((char*)memoryStringPointer, memoryStringSize, msgTable)) {
			Throw(kMoaErr_OutOfMem);
		}

		ThrowErr(registryEntryDictInterfacePointer->Put(kMoaDrDictType_MessageTable, memoryStringPointer, memoryStringSize, kMoaDrDictKey_MessageTable));
	}

	moa_catch
	moa_catch_end

	// always do this, whether there is an error or not
	if (memoryStringPointer) {
		pObj->pCalloc->NRFree(memoryStringPointer);
		memoryStringPointer = NULL;
	}

	moa_try_end
}




// this says that the Constructor/Deconstructor for
// the IMoaMmXScript interface provided by
// the Standard Xtra class will not do anything
STD_INTERFACE_CREATE_DESTROY(TStdXtra, IMoaMmXScript)

// this allows Director to
// Query and Release the IMoaMmXScript interface provided
// by the Standard Xtra class
BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaMmXScript)
END_DEFINE_CLASS_INTERFACE

// the IMoaMmXScript interface is expected
// to implement a method called Call which
// will be called by Director when
// a Lingo Script attempts to call
// one of the handlers
// defined in the Method Table which
// was registered in the Register method
// implemented by the IMoaRegister interface
// Director will call this method and
// pass in the argument for it, which is
// a pointer with data about the call, including
// which handler was called and the arguments passed
// to the handler from the Lingo Script that attempted
// to call it
STDMETHODIMP TStdXtra_IMoaMmXScript::Call(PMoaDrCallInfo callPtr) {
	moa_try

	ThrowNull(callPtr);

	// switch statement with all the handlers that may be called - we arrive here from Lingo first
	switch (callPtr->methodSelector) {
		case m_setTheMoviePath:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, THE_MOVIE_PATH_SIZE, (PMoaChar)theMoviePath));
		break;
		case m_setTheMovieName:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, THE_MOVIE_NAME_SIZE, (PMoaChar)theMovieName));
		break;
		case m_setTheEnvironment_shockMachine:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, &theEnvironment_shockMachine));
		break;
		case m_setTheEnvironment_shockMachineVersion:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, THE_ENVIRONMENT_SHOCK_MACHINE_VERSION_SIZE, (PMoaChar)theEnvironment_shockMachineVersion));
		break;
		case m_setThePlatform:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, THE_PLATFORM_SIZE, (PMoaChar)thePlatform));
		break;
		case m_setTheRunMode:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, THE_RUN_MODE_SIZE, (PMoaChar)theRunMode));
		break;
		case m_setTheEnvironment_productBuildVersion:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, THE_ENVIRONMENT_PRODUCT_BUILD_VERSION_SIZE, (PMoaChar)theEnvironment_productBuildVersion));
		break;
		case m_setTheProductVersion:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, THE_PRODUCT_VERSION_SIZE, (PMoaChar)theProductVersion));
		break;
		case m_setTheEnvironment_osVersion:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, THE_ENVIRONMENT_OS_VERSION_SIZE, (PMoaChar)theEnvironment_osVersion));
		break;
		case m_setTheMachineType:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, &theMachineType));
		break;
		case m_setExternalParam:
		// this one requires a bit of fiddling with strings first
		ThrowErr(SetExternalParam(callPtr, MODULE_DIRECTOR_API));
		break;
		case m_forceTheExitLock:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, &theExitLock));
		break;
		case m_forceTheSafePlayer:
		ThrowErr(Extender(callPtr, MODULE_DIRECTOR_API, &theSafePlayer));
		break;
		case m_disableGoToNetMovie:
		case m_disableGoToNetPage:
		// these guys are in the NetLingo Xtra
		ThrowErr(Extender(callPtr, MODULE_NET_LINGO_XTRA));
		break;
		case m_bugfixShockwave3DBadDriverList:
		// this one is in the Shockwave 3D Asset Xtra
		ThrowErr(Extender(callPtr, MODULE_SHOCKWAVE_3D_ASSET_XTRA));
	}
	moa_catch
	moa_catch_end
	moa_try_end
}




// these methods are called from the Call method
// implemented by the IMoaMmXScript interface
// and correspond to handlers in Lingo upon
// the Method Table being registered by
// the Register method implemented by
// the IMoaRegister interface
MoaError TStdXtra_IMoaMmXScript::Extender(PMoaDrCallInfo callPtr, MODULE module, PIMoaDrMovie moaDrMovieInterfacePointer) {
	moa_try

	ThrowNull(callPtr);
	ThrowNull(moaDrMovieInterfacePointer);

	if (!extender(callPtr->methodSelector, module, moaDrMovieInterfacePointer, pObj->moaMmValueInterfacePointer, pObj->pCallback)) {
		terminateCurrentProcess();
		Throw(kMoaErr_InternalError);
	}
	moa_catch
	moa_catch_end
	moa_try_end
}

MoaError TStdXtra_IMoaMmXScript::Extender(PMoaDrCallInfo callPtr, MODULE module) {
	PIMoaDrMovie moaDrMovieInterfacePointer = NULL;

	moa_try

	ThrowNull(callPtr);

	// get the Active Movie (so we can call a Lingo Handler in it if we need to)
	ThrowErr(pObj->moaDrPlayerInterfacePointer->GetActiveMovie(&moaDrMovieInterfacePointer));
	
	ThrowErr(Extender(callPtr, module, moaDrMovieInterfacePointer));

	moa_catch
	moa_catch_end

	if (moaDrMovieInterfacePointer) {
		moaDrMovieInterfacePointer->Release();
		moaDrMovieInterfacePointer = NULL;
	}

	moa_try_end
}

MoaError TStdXtra_IMoaMmXScript::Extender(PMoaDrCallInfo callPtr, MODULE module, PMoaLong property) {
	MoaMmValue argumentValue = kVoidMoaMmValueInitializer;

	moa_try

	ThrowNull(callPtr);
	ThrowNull(property);

	// this is an overload for the same method as the previous but with the PMoaLong type
	AccessArgByIndex(1, &argumentValue);
	ThrowErr(pObj->moaMmValueInterfacePointer->ValueToInteger(&argumentValue, property));

	ThrowErr(Extender(callPtr, module));

	moa_catch
	moa_catch_end
	moa_try_end
}

MoaError TStdXtra_IMoaMmXScript::Extender(PMoaDrCallInfo callPtr, MODULE module, MoaLong propertySize, PMoaChar property) {
	MoaMmValue argumentValue = kVoidMoaMmValueInitializer;

	moa_try

	ThrowNull(callPtr);
	ThrowNull(property);

	// same deal again, this time for the PMoaChar type
	AccessArgByIndex(1, &argumentValue);
	ThrowErr(pObj->moaMmValueInterfacePointer->ValueToString(&argumentValue, property, propertySize));
	// this is done by the extended code now
	//*(unsigned char*)property = (unsigned char)strlen((char*)(property + 1));

	ThrowErr(Extender(callPtr, module));

	moa_catch
	moa_catch_end
	moa_try_end
}

MoaError TStdXtra_IMoaMmXScript::SetExternalParam(PMoaDrCallInfo callPtr, MODULE module) {
	PIMoaDrMovie moaDrMovieInterfacePointer = NULL;

	MoaMmValue argumentValue = kVoidMoaMmValueInitializer;
	PMoaChar externalParamsOld = NULL;

	moa_try
	ThrowNull(callPtr);

	ThrowErr(pObj->moaDrPlayerInterfacePointer->GetActiveMovie(&moaDrMovieInterfacePointer));

	const size_t NAME_SIZE = 256;
	MoaChar name[NAME_SIZE] = "";

	AccessArgByIndex(1, &argumentValue);
	ThrowErr(pObj->moaMmValueInterfacePointer->ValueToString(&argumentValue, (PMoaChar)name, NAME_SIZE));

	const size_t VALUE_SIZE = 256;
	MoaChar value[VALUE_SIZE] = "";

	AccessArgByIndex(2, &argumentValue);
	ThrowErr(pObj->moaMmValueInterfacePointer->ValueToString(&argumentValue, (PMoaChar)value, VALUE_SIZE));

	// name must not be empty
	if (stringNullOrEmpty(name)) {
		Throw(kMoaErr_BadParam);
	}

	size_t externalParamsSizeOld = externalParamsSize;
	externalParamsOld = externalParams;

	// adds the Param to the "browser"
	// four strings, at least one of which must be at least one character
	if (externalParamsSizeOld > 4) {
		// find out if this External Param was set before
		// and if it was, unset it
		// start with first External Param Old Name
		PMoaChar externalParamOldName = externalParamsOld;
		size_t externalParamOldNameSize = stringSize(externalParamOldName);

		PMoaChar externalParamOldValue = NULL;
		size_t externalParamOldValueSize = 0;

		// empty values are allowed, but not empty names
		// while the External Param Old Name does not exceed the External Params Old Size
		while (externalParamOldName + externalParamOldNameSize <= externalParamsOld + externalParamsSizeOld && strlen(externalParamOldName)) {
			// go to next External Param Old Value
			externalParamOldValue = externalParamOldName + externalParamOldNameSize;
			externalParamOldValueSize = stringSize(externalParamOldValue);
			
			// if the External Param Old Value exceeds the External Params Old Size
			if (externalParamOldValue + externalParamOldValueSize > externalParamsOld + externalParamsSizeOld) {
				break;
			}

			// if the External Param Name equals an External Param Old Name
			if (stringEquals(name, externalParamOldName)) {
				// out with the old in with the new
				if (!memoryShift(externalParamsOld, externalParamsSizeOld, externalParamOldValue + externalParamOldValueSize, externalParamsOld + externalParamsSizeOld - externalParamOldValue - externalParamOldValueSize, externalParamOldNameSize + externalParamOldValueSize, false)) {
					// dangerous - memory is in unknown state, so quit
					externalParamOldName = NULL;
					externalParamOldValue = NULL;
					showLastError("Failed to Unset External Param");
					terminateCurrentProcess();
					Throw(kMoaErr_OutOfMem);
				}

				externalParamsSizeOld -= externalParamOldNameSize + externalParamOldValueSize;
				break;
			}
			
			// go to next External Param Old Name
			externalParamOldNameSize = stringSize(externalParamOldValue + externalParamOldValueSize);
			externalParamOldName = externalParamOldValue + externalParamOldValueSize;
		}

		externalParamOldName = NULL;
		externalParamOldValue = NULL;
		
		externalParamsSize = stringSize(name) + stringSize(value) + externalParamsSizeOld;
		externalParams = (PMoaChar)pObj->pCalloc->NRAlloc(externalParamsSize);
		ThrowNull(externalParams);

		if (memcpy_s(externalParams, externalParamsSize - stringSize(name) - stringSize(value) - 2, externalParamsOld, externalParamsSizeOld - 2)) {
			// dangerous - memory is in unknown state, so quit
			showLastError("Failed to Set External Params");
			terminateCurrentProcess();
			Throw(kMoaErr_OutOfMem);
		}

		if (strcpy_s(externalParams + externalParamsSize - stringSize(name) - stringSize(value) - 2, externalParamsSize - stringSize(value) - externalParamsSizeOld, name)) {
			// dangerous - string is in unknown state, so quit
			showLastError("Failed to Set External Param Name");
			terminateCurrentProcess();
			Throw(kMoaErr_OutOfMem);
		}

		if (strcpy_s(externalParams + externalParamsSize - stringSize(value) - 2, externalParamsSize - stringSize(name) - externalParamsSizeOld, value)) {
			// dangerous - string is in unknown state, so quit
			showLastError("Failed to Set External Param Value");
			terminateCurrentProcess();
			Throw(kMoaErr_OutOfMem);
		}
	} else {
		externalParamsSize = stringSize(name) + stringSize(value) + 2;
		externalParams = (PMoaChar)pObj->pCalloc->NRAlloc(externalParamsSize);
		ThrowNull(externalParams);

		if (strcpy_s(externalParams, externalParamsSize - stringSize(value), name)) {
			// dangerous - string is in unknown state, so quit
			showLastError("Failed to Set External Param Name");
			terminateCurrentProcess();
			Throw(kMoaErr_OutOfMem);
		}

		if (strcpy_s(externalParams + stringSize(name), externalParamsSize - stringSize(name), value)) {
			// dangerous - string is in unknown state, so quit
			showLastError("Failed to Set External Param Value");
			terminateCurrentProcess();
			Throw(kMoaErr_OutOfMem);
		}
	}

	*(PMoaChar)(externalParams + externalParamsSize - 1) = NULL;
	*(PMoaChar)(externalParams + externalParamsSize - 2) = NULL;

	ThrowErr(Extender(callPtr, module, moaDrMovieInterfacePointer));

	moa_catch
	moa_catch_end

	if (externalParamsOld) {
		pObj->pCalloc->NRFree(externalParamsOld);
		externalParamsOld = NULL;
	}

	// don't forget to brush your teeth
	if (moaDrMovieInterfacePointer) {
		moaDrMovieInterfacePointer->Release();
		moaDrMovieInterfacePointer = NULL;
	}

	moa_try_end
}
/* End Xtra */




/* Begin Extender */
bool getInterfaceModuleHandle(HMODULE &moduleHandle, MODULE module, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaCallback callbackPointer) {
	moduleHandle = NULL;

	if (!moaDrMovieInterfacePointer || !callbackPointer) {
		return false;
	}

	if (module == MODULE_DIRECTOR_API) {
		// turn the Director API Interface into a Module Handle
		PMoaVoid moaVoidPointer = *(PPMoaVoid)moaDrMovieInterfacePointer;

		if (moaVoidPointer) {
			bool result = false;

			if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCSTR)moaVoidPointer, &moduleHandle)) {
				goto error;
			}

			result = true;
			error:
			moaVoidPointer = NULL;
			return result;
		}
	} else {
		MoaError err = kMoaErr_BadClass;
		PIMoaMmXScript scriptXtraInstance = NULL;

		// load the Xtra Instance's Interface, then turn it into a Module Handle
		if (module == MODULE_NET_LINGO_XTRA) {
			err = callbackPointer->MoaCreateInstance(&CLSID_CNetLingo, &IID_IMoaMmXScript, (PPMoaVoid)&scriptXtraInstance);
		} else if (module == MODULE_SHOCKWAVE_3D_ASSET_XTRA) {
			err = callbackPointer->MoaCreateInstance(&CLSID_CShockwave3DAsset, &IID_IMoaMmXScript, (PPMoaVoid)&scriptXtraInstance);
		}

		if (err != kMoaErr_NoErr) {
			return false;
		}

		if (scriptXtraInstance) {
			bool result = false;

			PMoaVoid moaVoidPointer = *(PPMoaVoid)scriptXtraInstance;

			if (moaVoidPointer) {
				if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCSTR)moaVoidPointer, &moduleHandle)) {
					goto error2;
				}

				result = true;
				error2:
				moaVoidPointer = NULL;
			}

			err = scriptXtraInstance->Release();
			scriptXtraInstance = NULL;

			if (err != kMoaErr_NoErr) {
				result = false;
			}
			return result;
		}
	}
	return false;
}

struct ModuleDirectorVersionTest {
	RELATIVE_VIRTUAL_ADDRESS codeRelativeVirtualAddress;
	VIRTUAL_SIZE codeVirtualSize;
	CODE1* testedCode;
};

MODULE_DIRECTOR_VERSION getModuleDirectorVersion(HMODULE moduleHandle, const size_t MODULE_DIRECTOR_VERSION_TESTS_SIZE, ModuleDirectorVersionTest moduleDirectorVersionTests[]) {
	// performs a test to see if this is a supported Director version
	// it's a simple array of bytes search, nothing more
	// we support 8.5, 10 and 11.5, so we perform three tests
	for (size_t i = 0;i < MODULE_DIRECTOR_VERSION_TESTS_SIZE;i++) {
		if (moduleDirectorVersionTests[i].codeVirtualSize) {
			if (testCode(moduleHandle, moduleDirectorVersionTests[i].codeRelativeVirtualAddress, moduleDirectorVersionTests[i].codeVirtualSize, moduleDirectorVersionTests[i].testedCode)) {
				return (MODULE_DIRECTOR_VERSION)i;
			}
		}
	}
	return MODULE_DIRECTOR_INCOMPATIBLE;
}




// Extended Code Addresses
EXTENDED_CODE_ADDRESS setTheMoviePathExtendedCodeCompareAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setThePathNameExtendedCodeCompareAddress = 0x00000000;

EXTENDED_CODE_ADDRESS setTheMovieNameExtendedCodeCompareAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setTheMovieExtendedCodeCompareAddress = 0x00000000;

EXTENDED_CODE_ADDRESS setTheEnvironment_shockMachineExtendedCodeReturnAddress = 0x00000000;

EXTENDED_CODE_ADDRESS setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = 0x00000000;

EXTENDED_CODE_ADDRESS setThePlatformExtendedCodeCompareAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setThePlatformExtendedCodeReturnAddress = 0x00000000;

EXTENDED_CODE_ADDRESS setTheRunModeTheProductVersionExtendedCodeCompareAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setTheEnvironment_runModeExtendedCodeReturnAddress = 0x00000000;

EXTENDED_CODE_ADDRESS setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setTheEnvironment_productBuildVersionExtendedCodeSubroutineAddress = 0x00000000;

EXTENDED_CODE_ADDRESS setTheEnvironment_productVersionExtendedCodeReturnAddress = 0x00000000;

EXTENDED_CODE_ADDRESS setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = 0x00000000;

EXTENDED_CODE_ADDRESS setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setTheMachineTypeExtendedCodeSubroutineAddress = 0x00000000;

EXTENDED_CODE_ADDRESS setExternalParamNameExternalParamValueExtendedCodeCompareAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setExternalParamNameExtendedCodeReturnAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setExternalParamValueExtendedCodeReturnAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setExternalParamCountExtendedCodeReturnAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setExternalParamExtendedCodeSubroutineAddress = 0x00000000;
EXTENDED_CODE_ADDRESS setExternalParamExtendedCodeSubroutineAddress2 = 0x00000000;
EXTENDED_CODE_ADDRESS setExternalParamExtendedCodeSubroutineAddress3 = 0x00000000;
EXTENDED_CODE_ADDRESS setExternalParamExtendedCodeSubroutineAddress4 = 0x00000000;

EXTENDED_CODE_ADDRESS forceTheExitLockExtendedCodeReturnAddress = 0x00000000;

EXTENDED_CODE_ADDRESS forceTheSafePlayerExtendedCodeReturnAddress = 0x00000000;
EXTENDED_CODE_ADDRESS forceTheSafePlayerExtendedCodeSubroutineAddress = 0x00000000;
EXTENDED_CODE_ADDRESS forceTheSafePlayerExtendedCodeSubroutineAddress2 = 0x00000000;
EXTENDED_CODE_ADDRESS forceTheSafePlayerExtendedCodeSubroutineAddress3 = 0x00000000;
EXTENDED_CODE_ADDRESS forceTheSafePlayerExtendedCodeSubroutineAddress4 = 0x00000000;

EXTENDED_CODE_ADDRESS bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = 0x00000000;

EXTENDED_CODE_ADDRESS bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = 0x00000000;

EXTENDED_CODE_ADDRESS bugfixShockwave3DBadDriverList3ExtendedCodeReturnAddress = 0x00000000;

EXTENDED_CODE_ADDRESS theEnvironmentCompareAddress = 0x00000000;

EXTENDED_CODE_ADDRESS lingoCallCompareAddress = 0x00000000;
EXTENDED_CODE_ADDRESS lingoCallCompareAddress2 = 0x00000000;
EXTENDED_CODE_ADDRESS lingoCallCompareAddress3 = 0x00000000;

EXTENDED_CODE_ADDRESS exceptionHandlerSubroutineAddress = 0x00000000;

__declspec(naked) void setTheMoviePathExtendedCode8() {
	__asm {
		// backup EAX
		push eax;

		// don't set if EDI is null
		test edi, edi
		jz do_not_set;

		// compare to the moviePath
		mov eax, [esp + 00000478h];
		cmp eax, [setTheMoviePathExtendedCodeCompareAddress];
		jnz compare_to_the_pathname;

		// compare to Lingo Call for the moviePath
		mov eax, [esp + 000006F4h];
		cmp eax, [lingoCallCompareAddress];
		jnz do_not_set;
		jmp set;

		compare_to_the_pathname:
		cmp eax, [setThePathNameExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the pathName
		mov eax, [esp + 000005F0h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the string
		push [THE_MOVIE_PATH_SIZE]; // num
		lea ebx, [theMoviePath];
		push ebx; // source/str
		push edi; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop edi;

		// get string length of the moviePath
		call strlen;
		// pop str
		pop ebx;

		// set string length
		mov bl, al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		// EBP is a pointer to the string length
		mov [ebp], bl;
		jmp epilogue;

		do_not_set:
		pop eax;
		cmp ebx, 000000FFh;
		mov [ebx + ebp + 00000001h], 00000000h;
		jge set_max_string_length;
		mov [ebp], bl;
		jmp epilogue;

		set_max_string_length:
		mov [ebp], 000000FFh;

		epilogue:
		// repeat the epilogue of the subroutine
		pop edi;
		pop esi;
		pop ebp;
		pop ebx;
		add esp, 00000464h;
		retn 00000008h;
	}
}

__declspec(naked) void setTheMovieNameExtendedCode8() {
	__asm {
		// backup EAX
		push eax;
		// compare to the moviePath
		mov eax, [esp + 00000360h];
		cmp eax, [setTheMovieNameExtendedCodeCompareAddress];
		jnz compare_to_the_movie;

		// compare to Lingo Call for the moviePath
		mov eax, [esp + 000005DCh];
		cmp eax, [lingoCallCompareAddress];
		jnz do_not_set;
		jmp set;

		compare_to_the_movie:
		cmp eax, [setTheMovieExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the pathName
		mov eax, [esp + 000004D8h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the movieName
		push [THE_MOVIE_NAME_SIZE]; // num
		lea ecx, [theMovieName];
		push ecx; // source/str
		lea eax, [edi + 00000001h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop eax;

		// get string length of the movieName
		call strlen;
		// pop str
		pop ecx;

		// set string length
		mov [edi], al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		jmp epilogue;

		do_not_set:
		pop eax;
		cmp ebx, 000000FFh;
		mov [eax + edi + 00000001h], 00000000h;
		jge set_max_string_length;
		mov [edi], al;
		jmp epilogue;

		set_max_string_length:
		mov [edi], 000000FFh;

		epilogue:
		// repeat the epilogue of the subroutine
		pop edi;
		add esp, 00000358h;
		retn 00000008h;
	}
}

__declspec(naked) void setTheEnvironment_shockMachineExtendedCode8() {
	__asm {
		// backup EAX
		push eax;
		// compare to the environment.shockMachine
		mov eax, [esp + 00000230h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz do_not_set;

		// restore EAX
		pop eax;
		mov ebx, [theEnvironment_shockMachine];
		jmp[setTheEnvironment_shockMachineExtendedCodeReturnAddress];

		do_not_set:
		// restore EAX
		pop eax;
		test eax, eax;
		jz epilogue;
		mov ebx, [eax + 00000028h];

		epilogue:
		jmp[setTheEnvironment_shockMachineExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_shockMachineVersionExtendedCode8() {
	__asm {
		// compare to the environment.shockMachineVersion
		mov eax, [esp + 0000022Ch];
		cmp eax, [theEnvironmentCompareAddress];
		jnz epilogue;

		push[THE_ENVIRONMENT_SHOCK_MACHINE_VERSION_SIZE]; // num
		lea eax, [theEnvironment_shockMachineVersion];
		push eax; // source/str
		lea eax, [esp + 00000134h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		pop eax;
		pop eax;
		pop eax;

		epilogue:
		lea edx, [esp + 00000010h];
		lea eax, [esp + 0000012Ch];
		jmp[setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setThePlatformExtendedCode8() {
	__asm {
		// backup EAX in case this is not the platform or the environment.platform
		push eax;
		// compare to the platform
		mov eax, [esp + 00000004h];
		cmp eax, [setThePlatformExtendedCodeCompareAddress];
		jnz compare_to_the_environment_platform;

		// compare to Lingo Call for the platform
		mov eax, [esp + 00000280h];
		cmp eax, [lingoCallCompareAddress];
		jnz do_not_set;
		jmp set;

		compare_to_the_environment_platform:
		cmp eax, [setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		set:
		// restore EAX
		pop eax;
		lea eax, [thePlatform];
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		push eax;
		mov eax, [esp + 0000000Ch];
		push eax;
		call [setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress];
		retn 00000008h;
	}
}

__declspec(naked) void setTheRunModeExtendedCode8() {
	__asm {
		// backup EAX
		push eax;
		// compare to the runMode
		mov eax, [esp + 00000134h];
		cmp eax, [setTheRunModeTheProductVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the runMode
		mov eax, [esp + 000003B8h];
		cmp eax, [lingoCallCompareAddress];
		jnz do_not_set;

		// restore EAX
		pop eax;

		// copy the string
		push [THE_RUN_MODE_SIZE]; // num
		lea ebx, [theRunMode];
		push ebx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		pop edi;
		pop ebx;
		mov eax, esi;
		pop esi;
		pop ebp;
		add esp, 00000120h;
		retn 00000010h;
	}
}

__declspec(naked) void setTheEnvironment_runModeExtendedCode8() {
	__asm {
		lea eax, [esp + 00000024h];

		// copy the string
		push [THE_RUN_MODE_SIZE]; // num
		lea edx, [theRunMode];
		push edx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop edx;
		pop edx;

		lea edx, [esp + 00000010h];
		jmp [setTheEnvironment_runModeExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_productBuildVersionExtendedCode8() {
	__asm {
		call [setTheEnvironment_productBuildVersionExtendedCodeSubroutineAddress];
		// backup EAX
		push eax;
		// compare to the environment.productBuildVersion
		mov eax, [esp + 0000013Ch];
		cmp eax, [setTheEnvironment_productBuildVersionExtendedCodeCompareAddress];
		jnz epilogue;

		// copy the string
		push [THE_ENVIRONMENT_PRODUCT_BUILD_VERSION_SIZE]; // num
		lea eax, [theEnvironment_productBuildVersion];
		push eax; // source
		push esi; // destination
		call strncpy;
		pop esi;
		pop eax;
		pop eax;

		epilogue:
		// restore EAX
		pop eax;
		pop esi;
		retn 00000008h;
	}
}

__declspec(naked) void setTheProductVersionExtendedCode8() {
	__asm {
		// compare to the productVersion
		mov eax, [esp + 00000128h];
		cmp eax, [setTheRunModeTheProductVersionExtendedCodeCompareAddress];
		jnz epilogue;

		// compare to Lingo Call for the productVersion
		mov eax, [esp + 000003ACh];
		cmp eax, [lingoCallCompareAddress];
		jnz epilogue;

		// copy the string
		push [THE_PRODUCT_VERSION_SIZE]; // num
		lea eax, [theProductVersion];
		push eax; // source
		lea eax, [esp + 00000164h];
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;

		epilogue:
		mov eax, esi;
		pop esi;
		pop ebp;
		add esp, 00000120h;
		retn 00000010h;
	}
}

__declspec(naked) void setTheMachineTypeExtendedCode8() {
	__asm {
		// repeat the code from the getter
		call [setTheMachineTypeExtendedCodeSubroutineAddress];

		// backup EAX
		push eax;
		// compare to the machineType
		mov eax, [esp + 00000004h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the machineType
		mov eax, [esp + 0000005Ch];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		// restore EAX
		pop eax;
		mov eax, [theMachineType];
		retn 00000004h;

		do_not_set:
		// restore EAX
		pop eax;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamNameExtendedCode8() {
	__asm {
		// prologue of getter
		push esi;
		push edi;
		mov edi, [esp + 00000014h];
		mov dword ptr [esp + 00000008h], 00000002h;
		mov dword ptr [esp + 0000000Ch], 00000000h;

		// compare to externalParamCount
		mov eax, [esp + 00000010h];
		cmp eax, [setExternalParamNameExternalParamValueExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000ACh];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor esi, esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamNameExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		mov eax, [eax];
		test eax, eax;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov esi, eax;
		test esi, esi;
		jz epilogue;
		push esi;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamNameExtendedCodeReturnAddress];

		epilogue:
		// if they are not, then return
		lea edx, [esp + 00000008h];
		push edx;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress4];
		pop edi;
		pop esi;
		add esp, 00000004h;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamValueExtendedCode8() {
	__asm {
		// prologue of getter
		push ebx;
		mov ebx, [esp + 00000010h];
		push edi;
		mov dword ptr [esp + 00000008h], 00000002h;
		mov dword ptr [esp + 0000000Ch], 00000000h;

		// compare to externalParamCount
		mov eax, [esp + 00000010h];
		cmp eax, [setExternalParamNameExternalParamValueExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000ACh];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor edi, edi;
		// this is pushed here in the getter
		push esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamValueExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		mov eax, [eax];
		test eax, eax;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov edi, eax;
		test edi, edi;
		jz epilogue;
		push esi;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamValueExtendedCodeReturnAddress];

		epilogue:
		// if they are not, then return
		lea edx, [esp + 00000008h];
		push edx;
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress4];
		pop edi;
		pop ebx;
		add esp, 00000008h;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamCountExtendedCode8() {
	__asm {
		// prologue of getter
		push ebx;
		push esi;
		xor esi, esi;

		// compare to externalParamCount
		mov eax, [esp + 00000008h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000A0h];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor ebx, ebx;
		// this is pushed here in the getter
		push edi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamCountExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		mov [eax], eax;
		test eax, eax;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push [esp + 0000000Ch];
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov ebx, eax;
		test ebx, ebx;
		jz epilogue;
		push edi;
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamCountExtendedCodeReturnAddress];

		epilogue:
		// if they are not, then return
		mov eax, edi;
		pop edi;
		pop ebx;
		retn 00000004h;
	}
}

__declspec(naked) void forceTheExitLockExtendedCode8() {
	__asm {
		// we want Director to force the exit lock, so we don't need to compare to Lingo Call
		mov ecx, [theExitLock];
		jmp [forceTheExitLockExtendedCodeReturnAddress];
	}
}

__declspec(naked) void forceTheExitLockExtendedCode238() {
	__asm {
		// in addition to telling Lingo what the exitLock is, we also need it to have an effect
		mov eax, [theExitLock];
		retn 00000004h;
	}
}

__declspec(naked) void forceTheSafePlayerExtendedCode8() {
	__asm {
		// set up argument for notification
		xor eax, eax;
		mov edx, [ebx + 00000008h];
		lea edx, [edx + 00000020h];
		// SearchPathCallback
		push [forceTheSafePlayerExtendedCodeSubroutineAddress];
		// EAX is zero at this point
		cmp [theSafePlayer], eax;
		// if forcing the safePlayer to zero, jump
		jz after_notification_zero;

		// are we changing to one or are we already one
		cmp [edx], eax;
		// backup EDX
		push edx;
		// it's going to become one, so push the safePlayer SearchPathCallback instead
		mov edx, [forceTheSafePlayerExtendedCodeSubroutineAddress2];
		mov [esp + 00000004h], edx;
		jnz after_notification_one;

		// changing to one
		// backup EAX
		push eax;
		// show notification
		mov edi, [ecx + 00000010h];
		mov edi, [edi + 00000020h];
		test edi, edi;
		jnz before_notification;

		xor edi, edi;
		jmp before_notification2;

		before_notification:
		mov edi, [edi];

		before_notification2:
		push eax;
		push 00000021h;
		push edi;
		call [forceTheSafePlayerExtendedCodeSubroutineAddress3];
		// restore EAX
		pop eax;
		// change to one
		inc eax;

		after_notification_one:
		// already one
		// restore EDX
		pop edx;

		after_notification_zero:
		// set property
		mov [edx], eax;
		// 0 to 0, 1 to 2
		shl eax, 00000001h;
		push eax;
		call [forceTheSafePlayerExtendedCodeSubroutineAddress4];
		jmp [forceTheSafePlayerExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheMoviePathExtendedCode85() {
	__asm {
		// backup EAX
		push eax;

		// don't set if EDI is null
		test edi, edi
		jz do_not_set;

		// compare to the moviePath
		mov eax, [esp + 00000474h];
		cmp eax, [setTheMoviePathExtendedCodeCompareAddress];
		jnz compare_to_the_pathname;

		// compare to Lingo Call for the moviePath
		mov eax, [esp + 000006F0h];
		cmp eax, [lingoCallCompareAddress];
		jnz do_not_set;
		jmp set;

		compare_to_the_pathname:
		cmp eax, [setThePathNameExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the pathName
		mov eax, [esp + 000005F0h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the string
		push [THE_MOVIE_PATH_SIZE]; // num
		lea ebx, [theMoviePath];
		push ebx; // source/str
		push edi; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop edi;

		// get string length of the moviePath
		call strlen;
		// pop str
		pop ebx;

		// set string length
		mov bl, al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		// EAX is a pointer to the string length
		mov [eax], bl;
		jmp epilogue;

		do_not_set:
		pop eax;

		epilogue:
		// repeat the epilogue of the subroutine
		pop edi;
		pop esi;
		pop ebx;
		leave;
		retn 00000008h;
	}
}

__declspec(naked) void setTheMovieNameExtendedCode85() {
	__asm {
		// backup EAX
		push eax;
		// compare to the movieName
		mov eax, [esp + 00000364h];
		cmp eax, [setTheMovieNameExtendedCodeCompareAddress];
		jnz compare_to_the_movie;

		// compare to Lingo Call for the movieName
		mov eax, [esp + 000005E0h];
		cmp eax, [lingoCallCompareAddress];
		jnz epilogue;
		jmp set;

		compare_to_the_movie:
		cmp eax, [setTheMovieExtendedCodeCompareAddress];
		jnz epilogue;

		// compare to Lingo Call for the movie
		mov eax, [esp + 000004E0h];
		cmp eax, [lingoCallCompareAddress2];
		jnz epilogue;

		set:
		// copy the movieName
		push [THE_MOVIE_NAME_SIZE]; // num
		lea ecx, [theMovieName];
		push ecx; // source/str
		lea eax, [esi + 00000001h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop eax;

		// get string length of the movieName
		call strlen;
		// pop str
		pop ecx;

		// set string length
		mov [esi], al;
		// pop num
		pop eax;

		epilogue:
		// restore EAX
		pop eax;
		// repeat the epilogue of the subroutine
		pop esi;
		leave;
		retn 00000008h;
	}
}

__declspec(naked) void setTheEnvironment_shockMachineExtendedCode85() {
	__asm {
		// backup EAX
		push eax;
		// compare to the environment.shockMachine
		mov eax, [esp + 00000334h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz do_not_set;

		// restore EAX
		pop eax;
		mov eax, [theEnvironment_shockMachine];
		jmp[setTheEnvironment_shockMachineExtendedCodeReturnAddress];

		do_not_set:
		// restore EAX
		pop eax;
		test eax, eax;
		jz set_zero;
		mov eax, [eax + 00000028h];
		jmp[setTheEnvironment_shockMachineExtendedCodeReturnAddress];
		set_zero:
		mov eax, [ebp + 0000000Ch];
		jmp[setTheEnvironment_shockMachineExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_shockMachineVersionExtendedCode85() {
	__asm {
		// compare to the environment.shockMachineVersion
		mov eax, [esp + 00000330h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz epilogue;

		push[THE_ENVIRONMENT_SHOCK_MACHINE_VERSION_SIZE]; // num
		lea eax, [theEnvironment_shockMachineVersion];
		push eax; // source/str
		lea eax, [ebp - 00000218h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		pop eax;
		pop eax;
		pop eax;

		epilogue:
		lea eax, [ebp - 00000008h];
		push eax;
		lea eax, [ebp - 00000218h];
		jmp[setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setThePlatformExtendedCode85() {
	__asm {
		// backup EAX in case this is not the platform or the environment.platform
		push eax;
		// compare to the platform
		mov eax, [esp + 00000408h];
		cmp eax, [setThePlatformExtendedCodeCompareAddress];
		jnz compare_to_the_environment_platform;

		// compare to Lingo Call for the platform
		mov eax, [esp + 00000688h];
		cmp eax, [lingoCallCompareAddress];
		jnz do_not_set;
		jmp set;

		compare_to_the_environment_platform:
		cmp eax, [setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		set:
		// restore EAX
		pop eax;
		lea eax, [thePlatform];
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		push eax;
		push [ebp + 0000000Ch];
		call [setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress];

		jmp [setThePlatformExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheRunModeExtendedCode85() {
	__asm {
		// backup EAX
		push eax;
		// compare to the runMode
		mov eax, [esp + 00000134h];
		cmp eax, [setTheRunModeTheProductVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the runMode
		mov eax, [esp + 000003B8h];
		cmp eax, [lingoCallCompareAddress];
		jnz do_not_set;

		// restore EAX
		pop eax;

		// copy the string
		push [THE_RUN_MODE_SIZE]; // num
		lea ebx, [theRunMode];
		push ebx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		mov eax, edi;
		pop edi;
		pop esi;
		pop ebx;
		leave;
		retn 00000010h;
	}
}

__declspec(naked) void setTheEnvironment_runModeExtendedCode85() {
	__asm {
		push ecx;

		// copy the string
		push [THE_RUN_MODE_SIZE]; // num
		lea ecx, [theRunMode];
		push ecx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;
		pop ecx;

		lea eax, [ebp - 00000008h];
		push eax;
		lea eax, [ebp - 00000118h];
		jmp [setTheEnvironment_runModeExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_productBuildVersionExtendedCode85() {
	__asm {
		// compare to the environment.productBuildVersion
		mov eax, [esp + 00000130h];
		cmp eax, [setTheEnvironment_productBuildVersionExtendedCodeCompareAddress];
		jnz epilogue;

		// copy the string
		push [THE_ENVIRONMENT_PRODUCT_BUILD_VERSION_SIZE]; // num
		lea eax, [theEnvironment_productBuildVersion];
		push eax; // source
		push [ebp + 00000010h]; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;

		epilogue:
		mov eax, edi;
		pop edi;
		pop esi;
		pop ebx;
		leave;
		retn 00000010h;
	}
}

__declspec(naked) void setTheProductVersionExtendedCode85() {
	__asm {
		// compare to the productVersion
		mov eax, [esp + 00000130h];
		cmp eax, [setTheRunModeTheProductVersionExtendedCodeCompareAddress];
		jnz epilogue;

		// compare to Lingo Call for the productVersion
		mov eax, [esp + 000003B4h];
		cmp eax, [lingoCallCompareAddress];
		jnz epilogue;

		// copy the string
		push [THE_PRODUCT_VERSION_SIZE]; // num
		lea eax, [theProductVersion];
		push eax; // source
		push [ebp + 00000010h]; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;

		epilogue:
		mov eax, edi;
		pop edi;
		pop esi;
		pop ebx;
		leave;
		retn 00000010h;
	}
}

__declspec(naked) void setTheEnvironment_productVersionExtendedCode85() {
	__asm {
		// copy the string
		push [THE_PRODUCT_VERSION_SIZE]; // num
		lea eax, [theProductVersion];
		push eax; // source
		lea eax, [ebp - 00000118h];
		push eax; // destination
		call strncpy;
		// pop destination
		pop eax;
		// pop source
		pop eax;
		// pop num
		pop eax;

		lea eax, [ebp - 00000008h];
		push eax;
		lea eax, [ebp - 00000118h];
		jmp [setTheEnvironment_productVersionExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_osVersionExtendedCode85() {
	__asm {
		// compare to environment.osVersion
		mov eax, [esp + 00000404h];
		cmp eax, [setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		lea eax, [theEnvironment_osVersion];
		jmp epilogue;

		do_not_set:
		lea eax, [ebp - 00000400h];

		epilogue:
		push eax;
		push [ebp + 00000010h];
		call [setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress];
		leave;
		retn 0000000Ch;
	}
}

__declspec(naked) void setTheMachineTypeExtendedCode85() {
	__asm {
		// repeat the code from the getter
		call [setTheMachineTypeExtendedCodeSubroutineAddress];

		// backup EAX
		push eax;
		// compare to the machineType
		mov eax, [esp + 00000004h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the machineType
		mov eax, [esp + 00000060h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		// restore EAX
		pop eax;
		mov eax, [theMachineType];
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamNameExtendedCode85() {
	__asm {
		// prologue of getter
		and dword ptr [ebp - 00000004h], 00000000h;
		push esi;
		push edi;
		mov edi, [ebp + 00000008h];
		mov dword ptr [ebp - 00000008h], 00000002h;

		// compare to externalParamCount
		mov eax, [esp + 00000014h];
		cmp eax, [setExternalParamNameExternalParamValueExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000ACh];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor esi, esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamNameExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp [eax], 00000000h;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov esi, eax;
		test esi, esi;
		jz epilogue;
		push esi;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamNameExtendedCodeReturnAddress];

		epilogue:
		// if they are not, then return
		lea eax, [ebp - 00000008h];
		push eax;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress4];
		pop edi;
		pop esi;
		leave;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamValueExtendedCode85() {
	__asm {
		// prologue of getter
		and dword ptr [ebp - 00000004h], 00000000h;
		push ebx;
		push edi;
		mov edi, [ebp + 00000008h];
		mov dword ptr [ebp - 00000008h], 00000002h;

		// compare to externalParamCount
		mov eax, [esp + 00000014h];
		cmp eax, [setExternalParamNameExternalParamValueExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000ACh];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor ebx, ebx;
		// this is pushed here in the getter
		push esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamValueExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp [eax], 00000000h;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov ebx, eax;
		test ebx, ebx;
		jz epilogue;
		push esi;
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamValueExtendedCodeReturnAddress];

		epilogue:
		// if they are not, then return
		lea eax, [ebp - 00000008h];
		push eax;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress4];
		pop edi;
		pop ebx;
		leave;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamCountExtendedCode85() {
	__asm {
		// prologue of getter
		push ebx;
		push edi;
		xor edi, edi;

		// compare to externalParamCount
		mov eax, [esp + 00000008h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000A0h];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor ebx, ebx;
		// this is pushed here in the getter
		push esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamCountExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp [eax], edi;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push [esp + 0000000Ch];
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov ebx, eax;
		test ebx, ebx;
		jz epilogue;
		push esi;
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamCountExtendedCodeReturnAddress];

		epilogue:
		// if they are not, then return
		mov eax, edi;
		pop edi;
		pop ebx;
		retn 00000004h;
	}
}

#define forceTheExitLockExtendedCode85 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode2385 forceTheExitLockExtendedCode238

__declspec(naked) void forceTheSafePlayerExtendedCode85() {
	__asm {
		mov edx, [esi + 00000008h];
		lea edx, [edx + 00000020h];
		// SearchPathCallback
		push [forceTheSafePlayerExtendedCodeSubroutineAddress];
		// ECX is zero at this point
		cmp [theSafePlayer], ecx;
		// if forcing the safePlayer to zero, jump
		jz after_notification_zero;

		// are we changing to one or are we already one
		cmp [edx], ecx;
		// backup EDX
		push edx;
		// it's going to become one, so push the safePlayer SearchPathCallback instead
		mov edx, [forceTheSafePlayerExtendedCodeSubroutineAddress2];
		mov [esp + 00000004h], edx;
		jnz after_notification_one;

		// changing to one
		// backup ECX
		push ecx;
		// show notification
		mov eax, [eax + 00000004h];
		push ecx;
		push 00000022h;
		push [eax + 00000010h];
		call [forceTheSafePlayerExtendedCodeSubroutineAddress3];
		// restore ECX
		pop ecx;
		// change to one
		inc ecx;

		after_notification_one:
		// already one
		// restore EDX
		pop edx;

		after_notification_zero:
		// set property
		mov [edx], ecx;
		// 0 to 0, 1 to 2
		shl ecx, 00000001h;
		push ecx;
		call [forceTheSafePlayerExtendedCodeSubroutineAddress4];
		jmp [forceTheSafePlayerExtendedCodeReturnAddress];
	}
}

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode85() {
	__asm {
		mov esi, [ebp - 00000004h];
		add esi, 00000109h;
		jmp [bugfixShockwave3DBadDriverListExtendedCodeReturnAddress];
	}
}

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode285() {
	__asm {
		push 00000104h;
		push edi;
		mov [ebp - 00000014h], edi;
		jmp [bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress];
	}
}

#define setTheMoviePathExtendedCode851 setTheMoviePathExtendedCode85

#define setTheMovieNameExtendedCode851 setTheMovieNameExtendedCode85

#define setTheEnvironment_shockMachineExtendedCode851 setTheEnvironment_shockMachineExtendedCode85

#define setTheEnvironment_shockMachineVersionExtendedCode851 setTheEnvironment_shockMachineVersionExtendedCode85

#define setThePlatformExtendedCode851 setThePlatformExtendedCode85

#define setTheRunModeExtendedCode851 setTheRunModeExtendedCode85

#define setTheEnvironment_runModeExtendedCode851 setTheEnvironment_runModeExtendedCode85

#define setTheEnvironment_productBuildVersionExtendedCode851 setTheEnvironment_productBuildVersionExtendedCode85

#define setTheProductVersionExtendedCode851 setTheProductVersionExtendedCode85

#define setTheEnvironment_productVersionExtendedCode851 setTheEnvironment_productVersionExtendedCode85

#define setTheEnvironment_osVersionExtendedCode851 setTheEnvironment_osVersionExtendedCode85

#define setTheMachineTypeExtendedCode851 setTheMachineTypeExtendedCode85

#define setExternalParamNameExtendedCode851 setExternalParamNameExtendedCode85

#define setExternalParamValueExtendedCode851 setExternalParamValueExtendedCode85

#define setExternalParamCountExtendedCode851 setExternalParamCountExtendedCode85

#define forceTheExitLockExtendedCode851 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode23851 forceTheExitLockExtendedCode238

#define forceTheSafePlayerExtendedCode851 forceTheSafePlayerExtendedCode85

#define bugfixShockwave3DBadDriverListExtendedCode851 bugfixShockwave3DBadDriverListExtendedCode85

#define bugfixShockwave3DBadDriverListExtendedCode2851 bugfixShockwave3DBadDriverListExtendedCode285

#define setTheMoviePathExtendedCode9 setTheMoviePathExtendedCode85

#define setTheMovieNameExtendedCode9 setTheMovieNameExtendedCode85

#define setTheEnvironment_shockMachineExtendedCode9 setTheEnvironment_shockMachineExtendedCode85

#define setTheEnvironment_shockMachineVersionExtendedCode9 setTheEnvironment_shockMachineVersionExtendedCode85

#define setThePlatformExtendedCode9 setThePlatformExtendedCode85

#define setTheRunModeExtendedCode9 setTheRunModeExtendedCode85

#define setTheEnvironment_runModeExtendedCode9 setTheEnvironment_runModeExtendedCode85

#define setTheEnvironment_productBuildVersionExtendedCode9 setTheEnvironment_productBuildVersionExtendedCode85

#define setTheProductVersionExtendedCode9 setTheProductVersionExtendedCode85

#define setTheEnvironment_productVersionExtendedCode9 setTheEnvironment_productVersionExtendedCode85

#define setTheEnvironment_osVersionExtendedCode9 setTheEnvironment_osVersionExtendedCode85

#define setTheMachineTypeExtendedCode9 setTheMachineTypeExtendedCode85

#define setExternalParamNameExtendedCode9 setExternalParamNameExtendedCode85

#define setExternalParamValueExtendedCode9 setExternalParamValueExtendedCode85

#define setExternalParamCountExtendedCode9 setExternalParamCountExtendedCode85

#define forceTheExitLockExtendedCode9 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode239 forceTheExitLockExtendedCode238

#define forceTheSafePlayerExtendedCode9 forceTheSafePlayerExtendedCode85

#define bugfixShockwave3DBadDriverListExtendedCode9 bugfixShockwave3DBadDriverListExtendedCode85

#define bugfixShockwave3DBadDriverListExtendedCode29 bugfixShockwave3DBadDriverListExtendedCode285

__declspec(naked) void setTheMoviePathExtendedCode10() {
	__asm {
		// backup EAX
		push eax;

		// don't set if EDI is null
		test edi, edi
		jz do_not_set;

		// compare to the moviePath
		mov eax, [esp + 000006ACh - 00000010h];
		cmp eax, [setTheMoviePathExtendedCodeCompareAddress];
		jnz compare_to_the_pathname;

		// compare to Lingo Call for the moviePath
		mov eax, [esp + 0000093Ch];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__movie_path;
		jmp set;

		compare_to__movie_path:
		mov eax, [esp + 00000838h];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;
		jmp set;

		compare_to_the_pathname:
		cmp eax, [setThePathNameExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the pathName
		mov eax, [esp + 00000818h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the string
		push [THE_MOVIE_PATH_SIZE]; // num
		lea ebx, [theMoviePath];
		push ebx; // source/str
		push edi; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop edi;

		// get string length of the moviePath
		call strlen;
		// pop str
		pop ebx;

		// set string length
		mov bl, al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		// EAX is a pointer to the string length
		mov [eax], bl;
		jmp epilogue;

		do_not_set:
		pop eax;

		epilogue:
		// repeat the epilogue of the subroutine
		pop edi;
		pop esi;
		pop ebx;
		leave;
		retn 00000008h;
	}
}

__declspec(naked) void setTheMovieNameExtendedCode10() {
	__asm {
		// backup EAX
		push eax;
		// compare to the movieName
		mov eax, [esp + 0000058Ch];
		cmp eax, [setTheMovieNameExtendedCodeCompareAddress];
		jnz compare_to_the_movie;

		// compare to Lingo Call for the movieName
		mov eax, [esp + 0000082Ch];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__movie_name;
		jmp set;

		compare_to__movie_name:
		mov eax, [esp + 00000728h];
		cmp eax, [lingoCallCompareAddress3];
		jnz epilogue;
		jmp set;

		compare_to_the_movie:
		cmp eax, [setTheMovieExtendedCodeCompareAddress];
		jnz epilogue;

		// compare to Lingo Call for the movie
		mov eax, [esp + 00000708h];
		cmp eax, [lingoCallCompareAddress2];
		jnz epilogue;

		set:
		lea ecx, [theMovieName];

		// copy the movieName
		push [THE_MOVIE_NAME_SIZE]; // num
		push ecx; // source/str
		lea eax, [esi + 00000001h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop eax;

		// get string length of the movieName
		call strlen;
		// pop str
		pop ecx;

		// set string length
		mov [esi], al;
		// pop num
		pop eax;

		epilogue:
		// restore EAX
		pop eax;
		// repeat the epilogue of the subroutine
		pop esi;
		leave;
		retn 00000008h;
	}
}

__declspec(naked) void setTheEnvironment_shockMachineExtendedCode10() {
	__asm {
		// backup EAX
		push eax;
		// compare to the environment.shockMachine
		mov eax, [esp + 00000334h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz do_not_set;

		// restore EAX
		pop eax;
		mov eax, [theEnvironment_shockMachine];
		jmp[setTheEnvironment_shockMachineExtendedCodeReturnAddress];

		do_not_set:
		// restore EAX
		pop eax;
		test eax, eax;
		jz set_zero;
		mov eax, [eax + 00000028h];
		jmp[setTheEnvironment_shockMachineExtendedCodeReturnAddress];
		set_zero:
		mov eax, [ebp + 0000000Ch];
		jmp[setTheEnvironment_shockMachineExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_shockMachineVersionExtendedCode10() {
	__asm {
		// compare to the environment.shockMachineVersion
		mov eax, [esp + 00000330h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz epilogue;

		push[THE_ENVIRONMENT_SHOCK_MACHINE_VERSION_SIZE]; // num
		lea eax, [theEnvironment_shockMachineVersion];
		push eax; // source/str
		lea eax, [ebp - 00000218h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		pop eax;
		pop eax;
		pop eax;

		epilogue:
		lea eax, [ebp - 00000008h];
		push eax;
		lea eax, [ebp - 00000218h];
		jmp[setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setThePlatformExtendedCode10() {
	__asm {
		// backup EAX in case this is not the platform or the environment.platform
		push eax;
		// compare to the platform
		mov eax, [esp + 00000408h];
		cmp eax, [setThePlatformExtendedCodeCompareAddress];
		jnz compare_to_the_environment_platform;

		// compare to Lingo Call for the platform
		mov eax, [esp + 00000578h];
		cmp eax, [lingoCallCompareAddress];
		jnz do_not_set;
		jmp set;

		compare_to_the_environment_platform:
		cmp eax, [setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		set:
		// restore EAX
		pop eax;
		lea eax, [thePlatform];
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		push eax;
		push [ebp + 0000000Ch];
		call [setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress];

		jmp [setThePlatformExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheRunModeExtendedCode10() {
	__asm {
		// backup EAX
		push eax;
		// compare to the runMode
		mov eax, [esp + 00000134h];
		cmp eax, [setTheRunModeTheProductVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the runMode
		mov eax, [esp + 000003DCh];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__player_runmode;
		jmp set;

		compare_to__player_runmode:
		mov eax, [esp + 000002D4h];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;

		set:
		// restore EAX
		pop eax;

		// copy the string
		push [THE_RUN_MODE_SIZE]; // num
		lea ebx, [theRunMode];
		push ebx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		mov eax, esi;
		pop edi;
		pop esi;
		pop ebx;
		leave;
		retn 00000010h;
	}
}

__declspec(naked) void setTheEnvironment_runModeExtendedCode10() {
	__asm {
		push ecx;

		// copy the string
		push [THE_RUN_MODE_SIZE]; // num
		lea ecx, [theRunMode];
		push ecx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;
		pop ecx;

		lea eax, [ebp - 00000008h];
		push eax;
		lea eax, [ebp - 00000118h];
		jmp [setTheEnvironment_runModeExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_productBuildVersionExtendedCode10() {
	__asm {
		// backup EAX
		push eax;
		// compare to the environment.productBuildVersion
		mov eax, [esp + 00000134h];
		cmp eax, [setTheEnvironment_productBuildVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		// restore EAX
		pop eax;

		// copy the string
		push [THE_ENVIRONMENT_PRODUCT_BUILD_VERSION_SIZE]; // num
		lea ebx, [theEnvironment_productBuildVersion];
		push ebx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		mov eax, esi;
		pop edi;
		pop esi;
		pop ebx;
		leave;
		retn 00000010h;
	}
}

__declspec(naked) void setTheProductVersionExtendedCode10() {
	__asm {
		// compare to the productVersion
		mov eax, [esp + 00000130h];
		cmp eax, [setTheRunModeTheProductVersionExtendedCodeCompareAddress];
		jnz epilogue;

		// compare to Lingo Call for the productVersion
		mov eax, [esp + 000003D8h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__player_productversion;
		jmp set;

		compare_to__player_productversion:
		mov eax, [esp + 000002D0h];
		cmp eax, [lingoCallCompareAddress3];
		jnz epilogue;

		set:
		// copy the string
		push [THE_PRODUCT_VERSION_SIZE]; // num
		lea eax, [theProductVersion];
		push eax; // source
		push [ebp + 00000010h]; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;

		epilogue:
		mov eax, esi;
		pop edi;
		pop esi;
		pop ebx;
		leave;
		retn 00000010h;
	}
}

__declspec(naked) void setTheEnvironment_productVersionExtendedCode10() {
	__asm {
		// compare to the environment.productVersion
		mov eax, [esp + 00000330h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz epilogue;

		// copy the string
		push [THE_PRODUCT_VERSION_SIZE]; // num
		lea eax, [theProductVersion];
		push eax; // source
		lea eax, [ebp - 00000118h];
		push eax; // destination
		call strncpy;
		// pop destination
		pop eax;
		// pop source
		pop eax;
		// pop num
		pop eax;

		epilogue:
		lea eax, [ebp - 00000008h];
		push eax;
		lea eax, [ebp - 00000118h];
		jmp [setTheEnvironment_productVersionExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_osVersionExtendedCode10() {
	__asm {
		// compare to environment.osVersion
		mov eax, [esp + 00000404h];
		cmp eax, [setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		lea eax, [theEnvironment_osVersion];
		jmp epilogue;

		do_not_set:
		lea eax, [ebp - 00000400h];

		epilogue:
		push eax;
		push [ebp + 00000010h];
		call [setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress];
		leave;
		retn 0000000Ch;
	}
}

__declspec(naked) void setTheMachineTypeExtendedCode10() {
	__asm {
		// repeat the code from the getter
		call [setTheMachineTypeExtendedCodeSubroutineAddress];

		// backup EAX
		push eax;
		// compare to the machineType
		mov eax, [esp + 00000004h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the machineType
		mov eax, [esp + 00000060h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		// restore EAX
		pop eax;
		mov eax, [theMachineType];
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamNameExtendedCode10() {
	__asm {
		// prologue of getter
		and dword ptr [ebp - 00000004h], 00000000h;
		push esi;
		push edi;
		mov edi, [ebp + 00000008h];
		mov dword ptr [ebp - 00000008h], 00000002h;

		// compare to externalParamCount
		mov eax, [esp + 00000014h];
		cmp eax, [setExternalParamNameExternalParamValueExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000ACh];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor esi, esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamNameExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp [eax], 00000000h;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov esi, eax;
		test esi, esi;
		jz epilogue;
		push esi;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamNameExtendedCodeReturnAddress];

		epilogue:
		// if they are not, then return
		lea eax, [ebp - 00000008h];
		push eax;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress4];
		pop edi;
		pop esi;
		leave;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamValueExtendedCode10() {
	__asm {
		// prologue of getter
		and dword ptr [ebp - 00000004h], 00000000h;
		push ebx;
		push edi;
		mov edi, [ebp + 00000008h];
		mov dword ptr [ebp - 00000008h], 00000002h;

		// compare to externalParamCount
		mov eax, [esp + 00000014h];
		cmp eax, [setExternalParamNameExternalParamValueExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000ACh];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor ebx, ebx;
		// this is pushed here in the getter
		push esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamValueExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp [eax], 00000000h;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov ebx, eax;
		test ebx, ebx;
		jz epilogue;
		push esi;
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamValueExtendedCodeReturnAddress];

		epilogue:
		// if they are not, then return
		lea eax, [ebp - 00000008h];
		push eax;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress4];
		pop edi;
		pop ebx;
		leave;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamCountExtendedCode10() {
	__asm {
		// prologue of getter
		push ebx;
		push edi;
		xor edi, edi;

		// compare to externalParamCount
		mov eax, [esp + 00000008h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000A0h];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor ebx, ebx;
		// this is pushed here in the getter
		push esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamCountExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp [eax], edi;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push [esp + 0000000Ch];
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov ebx, eax;
		test ebx, ebx;
		jz epilogue;
		push esi;
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamCountExtendedCodeReturnAddress];

		epilogue:
		// if they are not, then return
		mov eax, edi;
		pop edi;
		pop ebx;
		retn 00000004h;
	}
}

#define forceTheExitLockExtendedCode10 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode2310 forceTheExitLockExtendedCode238

__declspec(naked) void forceTheSafePlayerExtendedCode10() {
	__asm {
		mov edx, [esi + 00000008h];
		lea edx, [edx + 00000020h];
		// SearchPathCallback
		push [forceTheSafePlayerExtendedCodeSubroutineAddress];
		// ECX is zero at this point
		cmp [theSafePlayer], ecx;
		// if forcing the safePlayer to zero, jump
		jz after_notification_zero;

		// are we changing to one or are we already one
		cmp [edx], ecx;
		// backup EDX
		push edx;
		// it's going to become one, so push the safePlayer SearchPathCallback instead
		mov edx, [forceTheSafePlayerExtendedCodeSubroutineAddress2];
		mov [esp + 00000004h], edx;
		jnz after_notification_one;

		// changing to one
		// backup ECX
		push ecx;
		// show notification
		mov eax, [eax + 00000004h];
		push ecx;
		push 00000022h;
		push [eax + 00000020h];
		call [forceTheSafePlayerExtendedCodeSubroutineAddress3];
		// restore ECX
		pop ecx;
		// change to one
		inc ecx;

		after_notification_one:
		// already one
		// restore EDX
		pop edx;

		after_notification_zero:
		// set property
		mov [edx], ecx;
		// 0 to 0, 1 to 2
		shl ecx, 00000001h;
		push ecx;
		call [forceTheSafePlayerExtendedCodeSubroutineAddress4];
		jmp [forceTheSafePlayerExtendedCodeReturnAddress];
	}
}

#define bugfixShockwave3DBadDriverListExtendedCode10 bugfixShockwave3DBadDriverListExtendedCode85

#define bugfixShockwave3DBadDriverListExtendedCode210 bugfixShockwave3DBadDriverListExtendedCode285

__declspec(naked) void setTheMoviePathExtendedCode101() {
	__asm {
		// backup EAX
		push eax;

		// don't set if EDI is null
		test edi, edi
		jz do_not_set;

		// compare to the moviePath
		mov eax, [esp + 000006A0h];
		cmp eax, [setTheMoviePathExtendedCodeCompareAddress];
		jnz compare_to_the_pathname;

		// compare to Lingo Call for the moviePath
		mov eax, [esp + 00000940h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__movie_path;
		jmp set;

		compare_to__movie_path:
		mov eax, [esp + 0000083Ch];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;
		jmp set;

		compare_to_the_pathname:
		cmp eax, [setThePathNameExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the pathName
		mov eax, [esp + 0000081Ch];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the string
		push [THE_MOVIE_PATH_SIZE]; // num
		lea ebx, [theMoviePath];
		push ebx; // source/str
		push edi; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop edi;

		// get string length of the moviePath
		call strlen;
		// pop str
		pop ebx;

		// set string length
		mov bl, al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		// EAX is a pointer to the string length
		mov [eax], bl;
		jmp epilogue;

		do_not_set:
		pop eax;

		epilogue:
		// repeat the epilogue of the subroutine
		pop edi;
		pop esi;
		pop ebx;
		leave;
		retn 00000008h;
	}
}

__declspec(naked) void setTheMovieNameExtendedCode101() {
	__asm {
		// backup EAX
		push eax;
		// compare to the movieName
		mov eax, [esp + 00000590h];
		cmp eax, [setTheMovieNameExtendedCodeCompareAddress];
		jnz compare_to_the_movie;

		// compare to Lingo Call for the movieName
		mov eax, [esp + 00000830h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__movie_name;
		jmp set;

		compare_to__movie_name:
		mov eax, [esp + 0000072Ch];
		cmp eax, [lingoCallCompareAddress3];
		jnz epilogue;
		jmp set;

		compare_to_the_movie:
		cmp eax, [setTheMovieExtendedCodeCompareAddress];
		jnz epilogue;

		// compare to Lingo Call for the movie
		mov eax, [esp + 0000070Ch];
		cmp eax, [lingoCallCompareAddress2];
		jnz epilogue;

		set:
		// copy the movieName
		push [THE_MOVIE_NAME_SIZE]; // num
		lea ecx, [theMovieName];
		push ecx; // source/str
		lea eax, [esi + 00000001h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop eax;

		// get string length of the movieName
		call strlen;
		// pop str
		pop ecx;

		// set string length
		mov [esi], al;
		// pop num
		pop eax;

		epilogue:
		// restore EAX
		pop eax;
		// repeat the epilogue of the subroutine
		pop esi;
		leave;
		retn 00000008h;
	}
}

#define setTheEnvironment_shockMachineExtendedCode101 setTheEnvironment_shockMachineExtendedCode10

#define setTheEnvironment_shockMachineVersionExtendedCode101 setTheEnvironment_shockMachineVersionExtendedCode10

#define setThePlatformExtendedCode101 setThePlatformExtendedCode10

#define setTheRunModeExtendedCode101 setTheRunModeExtendedCode10

#define setTheEnvironment_runModeExtendedCode101 setTheEnvironment_runModeExtendedCode10

#define setTheEnvironment_productBuildVersionExtendedCode101 setTheEnvironment_productBuildVersionExtendedCode10

#define setTheProductVersionExtendedCode101 setTheProductVersionExtendedCode10

#define setTheEnvironment_productVersionExtendedCode101 setTheEnvironment_productVersionExtendedCode10

#define setTheEnvironment_osVersionExtendedCode101 setTheEnvironment_osVersionExtendedCode10

#define setTheMachineTypeExtendedCode101 setTheMachineTypeExtendedCode10

#define setExternalParamNameExtendedCode101 setExternalParamNameExtendedCode10

#define setExternalParamValueExtendedCode101 setExternalParamValueExtendedCode10

#define setExternalParamCountExtendedCode101 setExternalParamCountExtendedCode10

#define forceTheExitLockExtendedCode101 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode23101 forceTheExitLockExtendedCode238

#define forceTheSafePlayerExtendedCode101 forceTheSafePlayerExtendedCode10

#define bugfixShockwave3DBadDriverListExtendedCode101 bugfixShockwave3DBadDriverListExtendedCode85

#define bugfixShockwave3DBadDriverListExtendedCode2101 bugfixShockwave3DBadDriverListExtendedCode285

#define setTheMoviePathExtendedCode1011 setTheMoviePathExtendedCode101

#define setTheMovieNameExtendedCode1011 setTheMovieNameExtendedCode101

#define setTheEnvironment_shockMachineExtendedCode1011 setTheEnvironment_shockMachineExtendedCode10

#define setTheEnvironment_shockMachineVersionExtendedCode1011 setTheEnvironment_shockMachineVersionExtendedCode10

#define setThePlatformExtendedCode1011 setThePlatformExtendedCode10

#define setTheRunModeExtendedCode1011 setTheRunModeExtendedCode10

#define setTheEnvironment_runModeExtendedCode1011 setTheEnvironment_runModeExtendedCode10

#define setTheEnvironment_productBuildVersionExtendedCode1011 setTheEnvironment_productBuildVersionExtendedCode10

#define setTheProductVersionExtendedCode1011 setTheProductVersionExtendedCode10

#define setTheEnvironment_productVersionExtendedCode1011 setTheEnvironment_productVersionExtendedCode10

#define setTheEnvironment_osVersionExtendedCode1011 setTheEnvironment_osVersionExtendedCode10

#define setTheMachineTypeExtendedCode1011 setTheMachineTypeExtendedCode10

#define setExternalParamNameExtendedCode1011 setExternalParamNameExtendedCode10

#define setExternalParamValueExtendedCode1011 setExternalParamValueExtendedCode10

#define setExternalParamCountExtendedCode1011 setExternalParamCountExtendedCode10

#define forceTheExitLockExtendedCode1011 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode231011 forceTheExitLockExtendedCode238

#define forceTheSafePlayerExtendedCode1011 forceTheSafePlayerExtendedCode10

#define bugfixShockwave3DBadDriverListExtendedCode1011 bugfixShockwave3DBadDriverListExtendedCode85

#define bugfixShockwave3DBadDriverListExtendedCode21011 bugfixShockwave3DBadDriverListExtendedCode285

__declspec(naked) void setTheMoviePathExtendedCode11() {
	__asm {
		// backup EAX
		push eax;
		// compare to the moviePath
		mov eax, [esp + 00001478h];
		cmp eax, [setTheMoviePathExtendedCodeCompareAddress];
		jnz compare_to_the_pathname;

		// compare to Lingo Call for the moviePath
		mov eax, [esp + 00001D14h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__movie_path;
		jmp set;

		compare_to__movie_path:
		mov eax, [esp + 00001900h];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;
		jmp set;

		compare_to_the_pathname:
		cmp eax, [setThePathNameExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the pathName
		mov eax, [esp + 000018E4h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the string
		push [THE_MOVIE_PATH_SIZE]; // num
		lea ebx, [theMoviePath];
		push ebx; // source/str
		lea eax, [ebp + 00000001h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop eax;

		// get string length of the moviePath
		call strlen;
		// pop str
		pop ebx;

		// set string length
		mov bl, al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		// EBP is a pointer to the string length
		mov [ebp], bl;
		jmp epilogue;

		do_not_set:
		pop eax;
		cmp ebx, 000000FFh;
		mov [ebx + ebp + 00000001h], 00000000h;
		jge set_max_string_length;
		mov [ebp], bl;
		jmp epilogue;

		set_max_string_length:
		mov [ebp], 000000FFh;

		epilogue:
		// repeat the epilogue of the subroutine
		pop edi;
		pop ebp;
		pop ebx;
		add esp, 00001468h;
		retn 00000008h;
	}
}

__declspec(naked) void setTheMovieNameExtendedCode11() {
	__asm {
		// backup EAX
		push eax;
		// compare to the movieName
		mov eax, [esp + 0000146Ch];
		cmp eax, [setTheMovieNameExtendedCodeCompareAddress];
		jnz compare_to_the_movie;

		// compare to Lingo Call for the movieName
		mov eax, [esp + 00001D08h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__movie_name;
		jmp set;

		compare_to__movie_name:
		mov eax, [esp + 000018F4h];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;
		jmp set;

		compare_to_the_movie:
		cmp eax, [setTheMovieExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the movie
		mov eax, [esp + 000018D8h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the movieName
		push [THE_MOVIE_NAME_SIZE]; // num
		lea ecx, [theMovieName];
		push ecx; // source/str
		lea eax, [edi + 00000001h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop eax;

		// get string length of the movieName
		call strlen;
		// pop str
		pop ecx;

		// set string length
		mov [edi], al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		jmp epilogue;

		do_not_set:
		pop eax;
		cmp eax, 000000FFh;
		mov [eax + edi + 00000001h], 00000000h;
		jge set_max_string_length;
		mov [edi], al;
		jmp epilogue;

		set_max_string_length:
		mov [edi], 000000FFh;

		epilogue:
		// repeat the epilogue of the subroutine
		pop edi;
		add esp, 00001464h;
		retn 00000008h;
	}
}

__declspec(naked) void setTheEnvironment_shockMachineExtendedCode11() {
	__asm {
		lea ecx, [esp + 0000000Ch];
		push ecx;

		push [theEnvironment_shockMachine];
		jmp [setTheEnvironment_shockMachineExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_shockMachineVersionExtendedCode11() {
	__asm {
		// backup EAX
		push eax;
		// compare to the environment.shockMachineVersion
		mov eax, [esp + 00000938h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz compare_to_the_environment_shockmachineversion_odd;
		jmp set;

		compare_to_the_environment_shockmachineversion_odd:
		mov eax, [esp + 00000934h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz do_not_set;

		set:
		push[THE_ENVIRONMENT_SHOCK_MACHINE_VERSION_SIZE]; // num
		lea eax, [theEnvironment_shockMachineVersion];
		push eax; // source/str
		lea eax, [esp + 00000438h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		pop eax;
		pop eax;
		pop eax;
		jmp epilogue;

		do_not_set:
		mov[esp + 00000430h], 00000000h;

		epilogue:
		// restore EAX
		pop eax;
		jmp[setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setThePlatformExtendedCode11() {
	__asm {
		// backup EAX in case this is not the platform or the environment.platform
		push eax;
		// compare to the platform
		mov eax, [esp + 00000404h];
		cmp eax, [setThePlatformExtendedCodeCompareAddress];
		jnz compare_to_the_environment_platform;

		// compare to Lingo Call for the platform
		mov eax, [esp + 00000870h];
		cmp eax, [lingoCallCompareAddress];
		jnz do_not_set;
		jmp set;

		compare_to_the_environment_platform:
		cmp eax, [setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		set:
		// restore EAX
		pop eax;
		lea eax, [thePlatform];
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		push eax;
		push ecx;
		call [setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress];

		jmp [setThePlatformExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheRunModeExtendedCode11() {
	__asm {
		// backup EAX
		push eax;
		// compare to the runMode
		mov eax, [esp + 0000042Ch];
		cmp eax, [setTheRunModeTheProductVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the runMode
		mov eax, [esp + 00000CC8h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__player_runmode;
		jmp set;

		compare_to__player_runmode:
		mov eax, [esp + 000008B4h];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;

		set:
		// restore EAX
		pop eax;

		// copy the string
		push [THE_RUN_MODE_SIZE]; // num
		lea ebx, [theRunMode];
		push ebx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		pop edi;
		pop esi;
		mov eax, ebp;
		pop ebp;
		pop ebx;
		add esp, 00000418h;
		retn 00000010h;
	}
}

__declspec(naked) void setTheEnvironment_runModeExtendedCode11() {
	__asm {
		// copy the string
		push [THE_RUN_MODE_SIZE]; // num
		lea edx, [theRunMode];
		push edx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;

		lea edx, [esp + 0000000Ch];
		push edx;
		lea eax, [esp + 00000024h];
		jmp [setTheEnvironment_runModeExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_productBuildVersionExtendedCode11() {
	__asm {
		// backup EAX
		push eax;
		// compare to the environment.productBuildVersion
		mov eax, [esp + 0000042Ch];
		cmp eax, [setTheEnvironment_productBuildVersionExtendedCodeCompareAddress];
		jnz compare_to_the_environment_productbuildversion_odd;
		jmp set;

		compare_to_the_environment_productbuildversion_odd:
		mov eax, [esp + 00000428h];
		cmp eax, [setTheEnvironment_productBuildVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		set:
		// restore EAX
		pop eax;

		// copy the string
		push [THE_ENVIRONMENT_PRODUCT_BUILD_VERSION_SIZE]; // num
		lea ebx, [theEnvironment_productBuildVersion];
		push ebx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		pop edi;
		pop esi;
		mov eax, ebp;
		pop ebp;
		pop ebx;
		add esp, 00000418h;
		retn 00000010h;
	}
}

__declspec(naked) void setTheProductVersionExtendedCode11() {
	__asm {
		// compare to the productVersion
		mov eax, [esp + 00000428h];
		cmp eax, [setTheRunModeTheProductVersionExtendedCodeCompareAddress];
		jnz epilogue;

		// compare to Lingo Call for the productVersion
		mov eax, [esp + 00000CC4h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__player_productversion;
		jmp set;

		compare_to__player_productversion:
		mov eax, [esp + 000008B0h];
		cmp eax, [lingoCallCompareAddress3];
		jnz epilogue;

		set:
		// copy the string
		push [THE_PRODUCT_VERSION_SIZE]; // num
		lea eax, [theProductVersion];
		push eax; // source
		lea eax, [esp + 0000045Ch];
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;

		epilogue:
		pop edi;
		pop esi;
		mov eax, ebp;
		pop ebp;
		pop ebx;
		add esp, 00000418h;
		retn 00000010h;
	}
}

__declspec(naked) void setTheEnvironment_productVersionExtendedCode11() {
	__asm {
		// backup EAX
		push eax;
		// compare to the environment.productVersion
		mov eax, [esp + 0000092Ch];
		cmp eax, [theEnvironmentCompareAddress];
		jnz compare_to_the_environment_productversion_odd;
		jmp set;

		compare_to_the_environment_productversion_odd:
		mov eax, [esp + 00000928h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz do_not_set;

		set:
		// restore EAX
		pop eax;
		lea edx, [esp + 00000020h];

		// copy the string
		push [THE_PRODUCT_VERSION_SIZE]; // num
		lea ecx, [theProductVersion];
		push ecx; // source
		push edx; // destination
		call strncpy;
		// pop destination
		pop edx;
		// pop source
		pop ecx;
		// pop num
		pop ecx;
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;
		lea edx, [esp + 00000020h];

		epilogue:
		lea ecx, [esp + 0000000Ch];
		push ecx;
		jmp [setTheEnvironment_productVersionExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_osVersionExtendedCode11() {
	__asm {
		// backup EAX
		push eax;
		// compare to environment.osVersion
		mov eax, [esp + 00000404h];
		cmp eax, [setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		lea edx, [theEnvironment_osVersion];
		jmp epilogue;

		do_not_set:
		lea edx, [esp];

		epilogue:
		// restore EAX
		pop eax;
		push edx;
		push eax;
		call [setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress];
		add esp, 00000400h;
		retn 0000000Ch;
	}
}

__declspec(naked) void setTheMachineTypeExtendedCode11() {
	__asm {
		// call not jmp, because otherwise we'll return before this code is run
		push edi;
		call [setTheMachineTypeExtendedCodeSubroutineAddress];

		// backup EAX
		push eax;
		// compare to the machineType
		mov eax, [esp + 00000004h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the machineType
		mov eax, [esp + 00000058h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		// restore EAX
		pop eax;
		mov eax, [theMachineType];
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamNameExtendedCode11() {
	__asm {
		// prologue of getter
		push edi;
		mov edi, [esp + 00000010h];
		mov dword ptr[esp + 00000004h], 00000002h;
		mov dword ptr[esp + 00000008h], 00000000h;

		// compare to externalParamCount
		mov eax, [esp + 0000000Ch];
		cmp eax, [setExternalParamNameExternalParamValueExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000ACh];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// pushed for no reason???
		push esi;
		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor esi, esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamNameExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp [eax], 00000000h;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push esi;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov esi, eax;
		test esi, esi;
		jz epilogue2;
		push esi;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamNameExtendedCodeReturnAddress];

		epilogue2:
		pop esi;

		epilogue:
		// if they are not, then return
		lea edx, [esp + 00000004h];
		push edx;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress4];
		pop edi;
		add esp, 00000008h;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamValueExtendedCode11() {
	__asm {
		// prologue of getter
		push edi;
		mov edi, [esp + 00000010h];
		mov dword ptr[esp + 00000004h], 00000002h;
		mov dword ptr[esp + 00000008h], 00000000h;

		// compare to externalParamCount
		mov eax, [esp + 0000000Ch];
		cmp eax, [setExternalParamNameExternalParamValueExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000ACh];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// also pushed here for no reason???
		push ebx;
		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor ebx, ebx;
		// this is pushed here in the getter
		push esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamValueExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp [eax], 00000000h;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push ebx;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov ebx, eax;
		test ebx, ebx;
		jz epilogue2;
		push esi;
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamValueExtendedCodeReturnAddress];

		epilogue2:
		pop ebx;

		epilogue:
		// if they are not, then return
		lea edx, [esp + 00000004h];
		push edx;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress4];
		pop edi;
		add esp, 00000008h;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamCountExtendedCode11() {
	__asm {
		// prologue of getter
		push esi;
		xor esi, esi;

		// compare to externalParamCount
		mov eax, [esp + 00000004h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000A0h];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// this is pushed here in the getter
		push ebx;
		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor ebx, ebx;
		// this is pushed here in the getter
		push edi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamCountExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		test eax, eax;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		mov eax, [esp + 00000008h];
		push ebx;
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov ebx, eax;
		test ebx, ebx;
		jz epilogue2;
		push edi;
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamCountExtendedCodeReturnAddress];

		epilogue2:
		pop ebx;

		epilogue:
		// if they are not, then return
		mov eax, esi;
		pop esi;
		retn 00000004h;
	}
}

#define forceTheExitLockExtendedCode11 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode2311 forceTheExitLockExtendedCode238

__declspec(naked) void forceTheSafePlayerExtendedCode11() {
	__asm {
		xor ecx, ecx;
		mov edx, [esi + 00000008h];
		lea edx, [edx + 00000020h];
		// SearchPathCallback
		push [forceTheSafePlayerExtendedCodeSubroutineAddress];
		// ECX is zero at this point
		cmp [theSafePlayer], ecx;
		// if forcing the safePlayer to zero, jump
		jz after_notification_zero;

		// are we changing to one or are we already one
		cmp [edx], ecx;
		// backup EDX
		push edx;
		// it's going to become one, so push the safePlayer SearchPathCallback instead
		mov edx, [forceTheSafePlayerExtendedCodeSubroutineAddress2];
		mov [esp + 00000004h], edx;
		jnz after_notification_one;

		// changing to one
		// backup ECX
		push ecx;
		// show notification
		mov eax, [eax + 00000004h];
		push ecx;
		push 00000022h;
		push [eax + 00000020h];
		call [forceTheSafePlayerExtendedCodeSubroutineAddress3];
		// restore ECX
		pop ecx;
		// change to one
		inc ecx;

		after_notification_one:
		// already one
		// restore EDX
		pop edx;

		after_notification_zero:
		// set property
		mov [edx], ecx;
		// 0 to 0, 1 to 2
		shl ecx, 00000001h;
		push ecx;
		call [forceTheSafePlayerExtendedCodeSubroutineAddress4];
		jmp [forceTheSafePlayerExtendedCodeReturnAddress];
	}
}

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode11() {
	__asm {
		mov esi, [ebp - 00000010h];
		add esi, 00000109h;
		jmp [bugfixShockwave3DBadDriverListExtendedCodeReturnAddress];
	}
}

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode211() {
	__asm {
		mov esi, 00000104h;
		push esi;
		push edi;
		mov [ebp - 00000020h], edi;
		jmp [bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress];
	}
}

#define setTheMoviePathExtendedCode1103 setTheMoviePathExtendedCode11

#define setTheMovieNameExtendedCode1103 setTheMovieNameExtendedCode11

#define setTheEnvironment_shockMachineExtendedCode1103 setTheEnvironment_shockMachineExtendedCode11

#define setTheEnvironment_shockMachineVersionExtendedCode1103 setTheEnvironment_shockMachineVersionExtendedCode11

#define setThePlatformExtendedCode1103 setThePlatformExtendedCode11

#define setTheRunModeExtendedCode1103 setTheRunModeExtendedCode11

#define setTheEnvironment_runModeExtendedCode1103 setTheEnvironment_runModeExtendedCode11

#define setTheEnvironment_productBuildVersionExtendedCode1103 setTheEnvironment_productBuildVersionExtendedCode11

#define setTheProductVersionExtendedCode1103 setTheProductVersionExtendedCode11

#define setTheEnvironment_productVersionExtendedCode1103 setTheEnvironment_productVersionExtendedCode11

#define setTheEnvironment_osVersionExtendedCode1103 setTheEnvironment_osVersionExtendedCode11

#define setTheMachineTypeExtendedCode1103 setTheMachineTypeExtendedCode11

#define setExternalParamNameExtendedCode1103 setExternalParamNameExtendedCode11

#define setExternalParamValueExtendedCode1103 setExternalParamValueExtendedCode11

#define setExternalParamCountExtendedCode1103 setExternalParamCountExtendedCode11

#define forceTheExitLockExtendedCode1103 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode231103 forceTheExitLockExtendedCode238

#define forceTheSafePlayerExtendedCode1103 forceTheSafePlayerExtendedCode11

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode1103() {
	__asm {
		mov edi, [ebp - 00000014h];
		add edi, 00000109h;
		jmp [bugfixShockwave3DBadDriverListExtendedCodeReturnAddress];
	}
}

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode21103() {
	__asm {
		mov edi, 00000104h;
		push edi;
		push ebx;
		mov [ebp - 00000014h], edi;
		jmp [bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress];
	}
}

#define setTheMoviePathExtendedCode115 setTheMoviePathExtendedCode11

#define setTheMovieNameExtendedCode115 setTheMovieNameExtendedCode11

#define setTheEnvironment_shockMachineExtendedCode115 setTheEnvironment_shockMachineExtendedCode11

#define setTheEnvironment_shockMachineVersionExtendedCode115 setTheEnvironment_shockMachineVersionExtendedCode11

#define setThePlatformExtendedCode115 setThePlatformExtendedCode11

#define setTheRunModeExtendedCode115 setTheRunModeExtendedCode11

#define setTheEnvironment_runModeExtendedCode115 setTheEnvironment_runModeExtendedCode11

#define setTheEnvironment_productBuildVersionExtendedCode115 setTheEnvironment_productBuildVersionExtendedCode11

#define setTheProductVersionExtendedCode115 setTheProductVersionExtendedCode11

#define setTheEnvironment_productVersionExtendedCode115 setTheEnvironment_productVersionExtendedCode11

#define setTheEnvironment_osVersionExtendedCode115 setTheEnvironment_osVersionExtendedCode11

#define setTheMachineTypeExtendedCode115 setTheMachineTypeExtendedCode11

#define setExternalParamNameExtendedCode115 setExternalParamNameExtendedCode11

#define setExternalParamValueExtendedCode115 setExternalParamValueExtendedCode11

#define setExternalParamCountExtendedCode115 setExternalParamCountExtendedCode11

#define forceTheExitLockExtendedCode115 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode23115 forceTheExitLockExtendedCode238

#define forceTheSafePlayerExtendedCode115 forceTheSafePlayerExtendedCode11

#define bugfixShockwave3DBadDriverListExtendedCode115 bugfixShockwave3DBadDriverListExtendedCode1103

#define bugfixShockwave3DBadDriverListExtendedCode2115 bugfixShockwave3DBadDriverListExtendedCode21103

#define setTheMoviePathExtendedCode1158 setTheMoviePathExtendedCode11

#define setTheMovieNameExtendedCode1158 setTheMovieNameExtendedCode11

#define setTheEnvironment_shockMachineExtendedCode1158 setTheEnvironment_shockMachineExtendedCode11

#define setTheEnvironment_shockMachineVersionExtendedCode1158 setTheEnvironment_shockMachineVersionExtendedCode11

#define setThePlatformExtendedCode1158 setThePlatformExtendedCode11

#define setTheRunModeExtendedCode1158 setTheRunModeExtendedCode11

#define setTheEnvironment_runModeExtendedCode1158 setTheEnvironment_runModeExtendedCode11

#define setTheEnvironment_productBuildVersionExtendedCode1158 setTheEnvironment_productBuildVersionExtendedCode11

#define setTheProductVersionExtendedCode1158 setTheProductVersionExtendedCode11

#define setTheEnvironment_productVersionExtendedCode1158 setTheEnvironment_productVersionExtendedCode11

#define setTheEnvironment_osVersionExtendedCode1158 setTheEnvironment_osVersionExtendedCode11

#define setTheMachineTypeExtendedCode1158 setTheMachineTypeExtendedCode11

#define setExternalParamNameExtendedCode1158 setExternalParamNameExtendedCode11

#define setExternalParamValueExtendedCode1158 setExternalParamValueExtendedCode11

#define setExternalParamCountExtendedCode1158 setExternalParamCountExtendedCode11

#define forceTheExitLockExtendedCode1158 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode231158 forceTheExitLockExtendedCode238

#define forceTheSafePlayerExtendedCode1158 forceTheSafePlayerExtendedCode11

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode1158() {
	__asm {
		call esi;
		cmp eax, ebx;
		jnz epilogue2;
		mov eax, [ebp - 00000010h];
		add eax, 00000214h;
		push eax;
		push [ebp - 00000018h];
		jmp[bugfixShockwave3DBadDriverListExtendedCodeReturnAddress];

	epilogue2:
		jmp[bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress];
	}
}

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode21158() {
	__asm {
		mov eax, [esp + 00000008h];
		mov [esp], eax;
		jmp [bugfixShockwave3DBadDriverList3ExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheMoviePathExtendedCode1159() {
	__asm {
		// backup EAX
		push eax;
		// compare to the moviePath
		mov eax, [esp + 00001478h];
		cmp eax, [setTheMoviePathExtendedCodeCompareAddress];
		jnz compare_to_the_pathname;

		// compare to Lingo Call for the moviePath
		mov eax, [esp + 00001D14h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__movie_path;
		jmp set;

		compare_to__movie_path:
		mov eax, [esp + 00001900h];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;
		jmp set;

		compare_to_the_pathname:
		cmp eax, [setThePathNameExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the pathName
		mov eax, [esp + 000018FCh];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the string
		push [THE_MOVIE_PATH_SIZE]; // num
		lea ebx, [theMoviePath];
		push ebx; // source/str
		lea eax, [ebp + 00000001h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop eax;

		// get string length of the moviePath
		call strlen;
		// pop str
		pop ebx;

		// set string length
		mov bl, al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		// EBP is a pointer to the string length
		mov [ebp], bl;
		jmp epilogue;

		do_not_set:
		pop eax;
		cmp ebx, 000000FFh;
		mov [ebx + ebp + 00000001h], 00000000h;
		jge set_max_string_length;
		mov [ebp], bl;
		jmp epilogue;

		set_max_string_length:
		mov [ebp], 000000FFh;

		epilogue:
		// repeat the epilogue of the subroutine
		pop edi;
		pop ebp;
		pop ebx;
		add esp, 00001468h;
		retn 00000008h;
	}
}

__declspec(naked) void setTheMovieNameExtendedCode1159() {
	__asm {
		// backup EAX
		push eax;
		// compare to the movieName
		mov eax, [esp + 0000146Ch];
		cmp eax, [setTheMovieNameExtendedCodeCompareAddress];
		jnz compare_to_the_movie;

		// compare to Lingo Call for the movieName
		mov eax, [esp + 00001D08h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__movie_name;
		jmp set;

		compare_to__movie_name:
		mov eax, [esp + 000018F4h];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;
		jmp set;

		compare_to_the_movie:
		cmp eax, [setTheMovieExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the movie
		mov eax, [esp + 000018F0h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the movieName
		push [THE_MOVIE_NAME_SIZE]; // num
		lea ecx, [theMovieName];
		push ecx; // source/str
		lea eax, [edi + 00000001h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop eax;

		// get string length of the movieName
		call strlen;
		// pop str
		pop ecx;

		// set string length
		mov [edi], al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		jmp epilogue;

		do_not_set:
		pop eax;
		cmp eax, 000000FFh;
		mov [eax + edi + 00000001h], 00000000h;
		jge set_max_string_length;
		mov [edi], al;
		jmp epilogue;

		set_max_string_length:
		mov [edi], 000000FFh;

		epilogue:
		// repeat the epilogue of the subroutine
		pop edi;
		add esp, 00001464h;
		retn 00000008h;
	}
}

#define setTheEnvironment_shockMachineExtendedCode1159 setTheEnvironment_shockMachineExtendedCode11

#define setTheEnvironment_shockMachineVersionExtendedCode1159 setTheEnvironment_shockMachineVersionExtendedCode11

#define setThePlatformExtendedCode1159 setThePlatformExtendedCode11

#define setTheRunModeExtendedCode1159 setTheRunModeExtendedCode11

#define setTheEnvironment_runModeExtendedCode1159 setTheEnvironment_runModeExtendedCode11

#define setTheEnvironment_productBuildVersionExtendedCode1159 setTheEnvironment_productBuildVersionExtendedCode11

#define setTheProductVersionExtendedCode1159 setTheProductVersionExtendedCode11

#define setTheEnvironment_productVersionExtendedCode1159 setTheEnvironment_productVersionExtendedCode11

#define setTheEnvironment_osVersionExtendedCode1159 setTheEnvironment_osVersionExtendedCode11

__declspec(naked) void setTheMachineTypeExtendedCode1159() {
	__asm {
		// call not jmp, because otherwise we'll return before this code is run
		push edi;
		call [setTheMachineTypeExtendedCodeSubroutineAddress];

		// backup EAX
		push eax;
		// compare to the machineType
		mov eax, [esp + 00000004h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the machineType
		mov eax, [esp + 00000070h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		// restore EAX
		pop eax;
		mov eax, [theMachineType];
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		retn 00000004h;
	}
}

#define setExternalParamNameExtendedCode1159 setExternalParamNameExtendedCode11

#define setExternalParamValueExtendedCode1159 setExternalParamValueExtendedCode11

#define setExternalParamCountExtendedCode1159 setExternalParamCountExtendedCode11

#define forceTheExitLockExtendedCode1159 forceTheExitLockExtendedCode8

#define forceTheExitLockExtendedCode231159 forceTheExitLockExtendedCode238

#define forceTheSafePlayerExtendedCode1159 forceTheSafePlayerExtendedCode11

#define bugfixShockwave3DBadDriverListExtendedCode1159 bugfixShockwave3DBadDriverListExtendedCode1158

#define bugfixShockwave3DBadDriverListExtendedCode21159 bugfixShockwave3DBadDriverListExtendedCode21158

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode1165() {
	__asm {
		call edi;
		cmp eax, esi;
		jnz epilogue2;
		mov eax, [ebp - 00000058h];
		add eax, 00000214h;
		push eax;
		push[ebp - 00000060h];
		jmp[bugfixShockwave3DBadDriverListExtendedCodeReturnAddress];

	epilogue2:
		jmp[bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress];
	}
}

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode21165() {
	__asm {
		mov eax, [esp + 00000008h];
		mov [esp], eax;
		jmp [bugfixShockwave3DBadDriverList3ExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheMoviePathExtendedCode12() {
	__asm {
		// backup EAX
		push eax;
		// compare to the moviePath
		mov eax, [esp + 00001480h];
		cmp eax, [setTheMoviePathExtendedCodeCompareAddress];
		jnz compare_to_the_pathname;

		// compare to Lingo Call for the moviePath
		mov eax, [esp + 00001D38h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__movie_path;
		jmp set;

		compare_to__movie_path:
		mov eax, [esp + 0000191Ch];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;
		jmp set;

		compare_to_the_pathname:
		cmp eax, [setThePathNameExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the pathName
		mov eax, [esp + 00001904h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the string
		push [THE_MOVIE_PATH_SIZE]; // num
		lea ebx, [theMoviePath];
		push ebx; // source/str
		lea eax, [esi + 00000001h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop eax;

		// get string length of the moviePath
		call strlen;
		// pop str
		pop ebx;

		// set string length
		mov bl, al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		// EBP is a pointer to the string length
		mov [esi], bl;
		jmp epilogue;

		do_not_set:
		pop eax;
		cmp ebx, 000000FFh;
		mov [ebx + esi + 00000001h], 00000000h;
		jge set_max_string_length;
		mov [esi], bl;
		jmp epilogue;

		set_max_string_length:
		mov [esi], 000000FFh;

		epilogue:
		// repeat the epilogue of the subroutine
		pop esi;
		pop ebx;
		mov ecx, [ebp - 00000004h];
		xor ecx, ebp;
		call [exceptionHandlerSubroutineAddress];
		mov esp, ebp;
		pop ebp;
		retn 00000008h;
	}
}

__declspec(naked) void setTheMovieNameExtendedCode12() {
	__asm {
		// backup EAX
		push eax;
		// compare to the movieName
		mov eax, [esp + 00001474h];
		cmp eax, [setTheMovieNameExtendedCodeCompareAddress];
		jnz compare_to_the_movie;

		// compare to Lingo Call for the movieName
		mov eax, [esp + 00001D2Ch];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__movie_name;
		jmp set;

		compare_to__movie_name:
		mov eax, [esp + 00001910h];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;
		jmp set;

		compare_to_the_movie:
		cmp eax, [setTheMovieExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the movie
		mov eax, [esp + 000018F8h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		set:
		// copy the movieName
		push [THE_MOVIE_NAME_SIZE]; // num
		lea ecx, [theMovieName];
		push ecx; // source/str
		lea eax, [esi + 00000001h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		// we need str to be pushed for strlen anyway, so just pop destination
		pop eax;

		// get string length of the movieName
		call strlen;
		// pop str
		pop ecx;

		// set string length
		mov [esi], al;
		// pop num
		pop eax;
		// restore EAX
		pop eax;
		jmp epilogue;

		do_not_set:
		pop eax;
		cmp eax, 000000FFh;
		mov [eax + esi + 00000001h], 00000000h;
		jge set_max_string_length;
		mov [esi], al;
		jmp epilogue;

		set_max_string_length:
		mov [esi], 000000FFh;

		epilogue:
		// repeat the epilogue of the subroutine
		pop esi;
		mov ecx, [ebp - 00000004h];
		xor ecx, ebp;
		call [exceptionHandlerSubroutineAddress];
		mov esp, ebp;
		pop ebp;
		retn 00000008h;
	}
}

__declspec(naked) void setTheEnvironment_shockMachineExtendedCode12() {
	__asm {
		lea edx, [ebp - 0000090Ch];
		push edx;

		push [theEnvironment_shockMachine];
		jmp [setTheEnvironment_shockMachineExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_shockMachineVersionExtendedCode12() {
	__asm {
		// backup EAX
		push eax;
		// compare to the environment.shockMachineVersion
		mov eax, [esp + 00000934h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz do_not_set;

		push[THE_ENVIRONMENT_SHOCK_MACHINE_VERSION_SIZE]; // num
		lea eax, [theEnvironment_shockMachineVersion];
		push eax; // source/str
		lea eax, [ebp - 00000504h];
		push eax; // destination
		call strncpy;
		// strncpy's calling convention means we need to now pop these things ourselves
		pop eax;
		pop eax;
		pop eax;
		jmp epilogue;

		do_not_set:
		mov[ebp - 00000504h], 00000000h;

		epilogue:
		// restore EAX
		pop eax;
		jmp[setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setThePlatformExtendedCode12() {
	__asm {
		// backup EAX in case this is not the platform or the environment.platform
		push eax;
		// compare to the platform
		mov eax, [esp + 00000414h];
		cmp eax, [setThePlatformExtendedCodeCompareAddress];
		jnz compare_to_the_environment_platform;

		// compare to Lingo Call for the platform
		mov eax, [esp + 00000894h];
		cmp eax, [lingoCallCompareAddress];
		jnz do_not_set;
		jmp set;

		compare_to_the_environment_platform:
		cmp eax, [setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		set:
		// restore EAX
		pop eax;
		lea eax, [thePlatform];
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		push eax;
		push edi;
		call [setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress];

		jmp [setThePlatformExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheRunModeExtendedCode12() {
	__asm {
		// backup EAX
		push eax;
		// compare to the runMode
		mov eax, [esp + 00000434h];
		cmp eax, [setTheRunModeTheProductVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the runMode
		mov eax, [esp + 00000CECh];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__player_runmode;
		jmp set;

		compare_to__player_runmode:
		mov eax, [esp + 000008D0h];
		cmp eax, [lingoCallCompareAddress3];
		jnz do_not_set;

		set:
		// restore EAX
		pop eax;

		// copy the string
		push [THE_RUN_MODE_SIZE]; // num
		lea ebx, [theRunMode];
		push ebx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		mov ecx, [ebp - 00000004h];
		mov eax, [ebp - 00000410h - 00000010h];
		pop edi;
		pop esi;
		xor ecx, ebp;
		pop ebx;
		call [exceptionHandlerSubroutineAddress];
		mov esp, ebp;
		pop ebp;
		retn 00000010h;
	}
}

__declspec(naked) void setTheEnvironment_runModeExtendedCode12() {
	__asm {
		// copy the string
		push [THE_RUN_MODE_SIZE]; // num
		lea edx, [theRunMode];
		push edx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;

		lea eax, [ebp - 0000090Ch];
		push eax;
		lea ecx, [ebp - 00000404h];
		jmp [setTheEnvironment_runModeExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_productBuildVersionExtendedCode12() {
	__asm {
		// backup EAX
		push eax;
		// compare to the environment.productBuildVersion
		mov eax, [esp + 00000434h];
		cmp eax, [setTheEnvironment_productBuildVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		// restore EAX
		pop eax;

		// copy the string
		push [THE_ENVIRONMENT_PRODUCT_BUILD_VERSION_SIZE]; // num
		lea ebx, [theEnvironment_productBuildVersion];
		push ebx; // source
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		mov ecx, [ebp - 00000004h];
		mov eax, [ebp - 00000410h - 00000010h];
		pop edi;
		pop esi;
		xor ecx, ebp;
		pop ebx;
		call [exceptionHandlerSubroutineAddress];
		mov esp, ebp;
		pop ebp;
		retn 00000010h;
	}
}

__declspec(naked) void setTheProductVersionExtendedCode12() {
	__asm {
		// compare to the productVersion
		mov eax, [esp + 00000430h];
		cmp eax, [setTheRunModeTheProductVersionExtendedCodeCompareAddress];
		jnz epilogue;

		// compare to Lingo Call for the productVersion
		mov eax, [esp + 00000CE8h];
		cmp eax, [lingoCallCompareAddress];
		jnz compare_to__player_productversion;
		jmp set;

		compare_to__player_productversion:
		mov eax, [esp + 000008CCh];
		cmp eax, [lingoCallCompareAddress3];
		jnz epilogue;

		set:
		// copy the string
		push [THE_PRODUCT_VERSION_SIZE]; // num
		lea eax, [theProductVersion];
		push eax; // source
		lea eax, [ebp + 00000028h];
		push eax; // destination
		call strncpy;
		pop eax;
		pop eax;
		pop eax;

		epilogue:
		mov ecx, [ebp - 00000004h];
		mov eax, [ebp - 00000410h - 00000010h];
		pop edi;
		pop esi;
		xor ecx, ebp;
		pop ebx;
		call [exceptionHandlerSubroutineAddress];
		mov esp, ebp;
		pop ebp;
		retn 00000010h;
	}
}

__declspec(naked) void setTheEnvironment_productVersionExtendedCode12() {
	__asm {
		// backup EAX
		push eax;
		// compare to the environment.productVersion
		mov eax, [esp + 00000928h];
		cmp eax, [theEnvironmentCompareAddress];
		jnz do_not_set;

		// restore EAX
		pop eax;
		lea eax, [ebp - 00000404h];

		// copy the string
		push [THE_PRODUCT_VERSION_SIZE]; // num
		lea ecx, [theProductVersion];
		push ecx; // source
		push eax; // destination
		call strncpy;
		// pop destination
		pop eax;
		// pop source
		pop ecx;
		// pop num
		pop ecx;
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;
		lea eax, [ebp - 00000404h];

		epilogue:
		lea edx, [ebp - 0000090Ch];
		push edx;
		jmp [setTheEnvironment_productVersionExtendedCodeReturnAddress];
	}
}

__declspec(naked) void setTheEnvironment_osVersionExtendedCode12() {
	__asm {
		// backup EAX
		push eax;
		// compare to environment.osVersion
		mov eax, [esp + 00000414h];
		cmp eax, [setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress];
		jnz do_not_set;

		lea ecx, [theEnvironment_osVersion];
		jmp epilogue;

		do_not_set:
		lea ecx, [ebp - 00000404h];

		epilogue:
		// restore EAX
		pop eax;
		push ecx;
		push esi;
		call [setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress];
		mov ecx, [ebp - 00000004h];
		pop edi;
		xor ecx, ebp;
		pop esi;
		call [exceptionHandlerSubroutineAddress];
		mov esp, ebp;
		pop ebp;
		retn 0000000Ch;
	}
}

__declspec(naked) void setTheMachineTypeExtendedCode12() {
	__asm {
		// call not jmp, because otherwise we'll return before this code is run
		push esi;
		call [setTheMachineTypeExtendedCodeSubroutineAddress];

		// backup EAX
		push eax;
		// compare to the machineType
		mov eax, [esp + 00000004h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for the machineType
		mov eax, [esp + 00000068h];
		cmp eax, [lingoCallCompareAddress2];
		jnz do_not_set;

		// restore EAX
		pop eax;
		mov eax, [theMachineType];
		jmp epilogue;

		do_not_set:
		// restore EAX
		pop eax;

		epilogue:
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamNameExtendedCode12() {
	__asm {
		// prologue of getter
		sub esp, 00000008h;
		push edi;
		mov edi, [ebp + 00000008h];
		mov dword ptr [ebp - 00000008h], 00000002h;
		mov dword ptr [ebp - 00000004h], 00000000h;

		// compare to externalParamCount
		mov eax, [esp + 00000010h];
		cmp eax, [setExternalParamNameExternalParamValueExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000ACh];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// pushed for no reason???
		push esi;
		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor esi, esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamNameExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp [eax], 00000000h;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push esi;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov esi, eax;
		test esi, esi;
		jz epilogue2;
		push esi;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamNameExtendedCodeReturnAddress];

		epilogue2:
		pop esi;

		epilogue:
		// if they are not, then return
		lea edx, [ebp - 000000008h];
		push edx;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress4];
		pop edi;
		mov esp, ebp;
		pop ebp;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamValueExtendedCode12() {
	__asm {
		// prologue of getter
		sub esp, 00000008h;
		push edi;
		mov edi, [ebp + 00000008h];
		mov dword ptr [ebp - 00000008h], 00000002h;
		mov dword ptr [ebp - 00000004h], 00000000h;

		// compare to externalParamCount
		mov eax, [esp + 00000010h];
		cmp eax, [setExternalParamNameExternalParamValueExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000ACh];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// also pushed here for no reason???
		push ebx;
		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor ebx, ebx;
		// this is pushed here in the getter
		push esi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamValueExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp [eax], 00000000h;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		push ebx;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov ebx, eax;
		test ebx, ebx;
		jz epilogue2;
		push esi;
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamValueExtendedCodeReturnAddress];

		epilogue2:
		pop ebx;

		epilogue:
		// if they are not, then return
		lea edx, [ebp - 00000008h];
		push edx;
		push edi;
		call [setExternalParamExtendedCodeSubroutineAddress4];
		pop edi;
		mov esp, ebp;
		pop ebp;
		retn 00000004h;
	}
}

__declspec(naked) void setExternalParamCountExtendedCode12() {
	__asm {
		// prologue of getter
		push esi;
		xor esi, esi;

		// compare to externalParamCount
		mov eax, [esp + 00000008h];
		cmp eax, [setTheMachineTypeExternalParamCountExtendedCodeCompareAddress];
		jnz do_not_set;

		// compare to Lingo Call for externalParamCount
		//mov eax, [esp + 000000A0h];
		//cmp eax, [setExternalParamExtendedCodeLingoCallCompareAddress];
		//jnz do_not_set;

		// this is pushed here in the getter
		push ebx;
		// null the imMemPointer so when it is unlocked it is ignored and no error is caused
		xor ebx, ebx;
		// this is pushed here in the getter
		push edi;
		// we don't care whether External Params are gettable because we're overriding them
		mov eax, externalParams;
		jmp [setExternalParamCountExtendedCodeReturnAddress];

		do_not_set:
		// now test if External Params are gettable
		mov eax, [setExternalParamExtendedCodeSubroutineAddress];
		cmp eax, esi;
		jz epilogue;

		// External Params are gettable
		// execute the code in the getter until we can jump back
		mov eax, [ebp + 00000008h];
		push ebx;
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress2];
		push eax;
		call [setExternalParamExtendedCodeSubroutineAddress];
		mov ebx, eax;
		test ebx, ebx;
		jz epilogue2;
		push edi;
		push ebx;
		call [setExternalParamExtendedCodeSubroutineAddress3];
		jmp [setExternalParamCountExtendedCodeReturnAddress];

		epilogue2:
		pop ebx;

		epilogue:
		// if they are not, then return
		mov eax, esi;
		pop esi;
		pop ebp;
		retn 00000004h;
	}
}

#define forceTheExitLockExtendedCode12 forceTheExitLockExtendedCode8

__declspec(naked) void forceTheExitLockExtendedCode212() {
	__asm {
		// in addition to telling Lingo what the exitLock is, we also need it to have an effect
		mov eax, [theExitLock];
		retn;
	}
}

#define forceTheExitLockExtendedCode312 forceTheExitLockExtendedCode238

__declspec(naked) void forceTheSafePlayerExtendedCode12() {
	__asm {
		push esi;
		mov eax, [ebp + 00000008h];
		mov eax, [eax + 00000008h];
		mov esi, [eax];
		mov esi, [esi + 0000051Ch];
		xor ecx, ecx;
		mov edx, [esi + 00000008h];
		lea edx, [edx + 00000020h];
		// SearchPathCallback
		push [forceTheSafePlayerExtendedCodeSubroutineAddress];
		// ECX is zero at this point
		cmp [theSafePlayer], ecx;
		// if forcing the safePlayer to zero, jump
		jz after_notification_zero;

		// are we changing to one or are we already one
		cmp [edx], ecx;
		// backup EDX
		push edx;
		// it's going to become one, so push the safePlayer SearchPathCallback instead
		mov edx, [forceTheSafePlayerExtendedCodeSubroutineAddress2];
		mov [esp + 00000004h], edx;
		jnz after_notification_one;

		// changing to one
		// backup ECX
		push ecx;
		// show notification
		mov eax, [eax + 00000004h];
		push ecx;
		push 00000022h;
		push [eax + 00000020h];
		call [forceTheSafePlayerExtendedCodeSubroutineAddress3];
		// restore ECX
		pop ecx;
		// change to one
		inc ecx;

		after_notification_one:
		// already one
		// restore EDX
		pop edx;

		after_notification_zero:
		// set property
		mov [edx], ecx;
		// 0 to 0, 1 to 2
		shl ecx, 00000001h;
		push ecx;
		call [forceTheSafePlayerExtendedCodeSubroutineAddress4];
		jmp [forceTheSafePlayerExtendedCodeReturnAddress];
	}
}

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode12() {
	__asm {
		call edi;
		cmp eax, esi;
		jnz epilogue2;
		mov eax, [ebp - 000003E4h];
		add eax, 00000214h;
		push eax;
		push[ebp - 000003ECh];
		jmp[bugfixShockwave3DBadDriverListExtendedCodeReturnAddress];

	epilogue2:
		jmp[bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress];
	}
}

__declspec(naked) void bugfixShockwave3DBadDriverListExtendedCode212() {
	__asm {
		mov eax, [esp + 00000008h];
		mov[esp], eax;
		jmp[bugfixShockwave3DBadDriverList3ExtendedCodeReturnAddress];
	}
}

__declspec(naked) void disableGoToNetThingExtendedCode() {
	__asm {
		retn 00000008h;
	}
}

bool extender(MoaMmSymbol methodSelector, MODULE module, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer, PIMoaCallback callbackPointer) {
	if (!moaDrMovieInterfacePointer || !moaMmValueInterfacePointer || !callbackPointer) {
		return false;
	}

	HMODULE moduleHandle = NULL;

	if (!getInterfaceModuleHandle(moduleHandle, module, moaDrMovieInterfacePointer, callbackPointer)) {
		callLingoAlertXtraMissing("Failed to Get Interface Module Handle", moaDrMovieInterfacePointer, moaMmValueInterfacePointer);
		return false;
	}

	if (!moduleHandle) {
		callLingoAlertXtraMissing("moduleHandle must not be NULL", moaDrMovieInterfacePointer, moaMmValueInterfacePointer);
		return false;
	}

	// this variable is true when we've successfully written everything required
	// the procedure is mostly the same for all modules
	bool codeExtended = false;
	MODULE_DIRECTOR_VERSION directorAPIDirectorVersion = MODULE_DIRECTOR_INCOMPATIBLE;
	MODULE_DIRECTOR_VERSION netLingoXtraDirectorVersion = MODULE_DIRECTOR_INCOMPATIBLE;
	MODULE_DIRECTOR_VERSION shockwave3DAssetXtraDirectorVersion = MODULE_DIRECTOR_INCOMPATIBLE;

	switch (module) {
		case MODULE_DIRECTOR_API:
		// test the code
		{
			const VIRTUAL_SIZE DIRECTOR_API_VERSION_8_TESTED_CODE_SIZE = 3;
			CODE1 directorAPIVersion8TestedCode[DIRECTOR_API_VERSION_8_TESTED_CODE_SIZE] = {0xFF, 0x14, 0x87};

			const VIRTUAL_SIZE DIRECTOR_API_VERSION_85_TESTED_CODE_SIZE = 19;
			CODE1 directorAPIVersion85TestedCode[DIRECTOR_API_VERSION_85_TESTED_CODE_SIZE] = {0x8D, 0x48, 0xFF, 0x81, 0xF9, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x87, 0x9E, 0x01, 0x00, 0x00, 0x33, 0xD2, 0x8A, 0x91};

			#define DIRECTOR_API_VERSION_851_TESTED_CODE_SIZE DIRECTOR_API_VERSION_85_TESTED_CODE_SIZE
			#define directorAPIVersion851TestedCode directorAPIVersion85TestedCode

			#define DIRECTOR_API_VERSION_9_TESTED_CODE_SIZE DIRECTOR_API_VERSION_85_TESTED_CODE_SIZE
			#define directorAPIVersion9TestedCode directorAPIVersion85TestedCode

			const VIRTUAL_SIZE DIRECTOR_API_VERSION_10_TESTED_CODE_SIZE = 18;
			CODE1 directorAPIVersion10TestedCode[DIRECTOR_API_VERSION_10_TESTED_CODE_SIZE] = {0x8D, 0x48, 0xFF, 0x81, 0xF9, 0xF2, 0x00, 0x00, 0x00, 0x0F, 0x87, 0x7B, 0x22, 0x00, 0x00, 0xFF, 0x24, 0x8D};

			#define DIRECTOR_API_VERSION_101_TESTED_CODE_SIZE DIRECTOR_API_VERSION_10_TESTED_CODE_SIZE
			#define directorAPIVersion101TestedCode directorAPIVersion10TestedCode

			#define DIRECTOR_API_VERSION_1011_TESTED_CODE_SIZE DIRECTOR_API_VERSION_10_TESTED_CODE_SIZE
			#define directorAPIVersion1011TestedCode directorAPIVersion10TestedCode

			const VIRTUAL_SIZE DIRECTOR_API_VERSION_11_TESTED_CODE_SIZE = 15;
			CODE1 directorAPIVersion11TestedCode[DIRECTOR_API_VERSION_11_TESTED_CODE_SIZE] = {0x81, 0xF9, 0xF2, 0x00, 0x00, 0x00, 0x0F, 0x87, 0x72, 0x1F, 0x00, 0x00, 0x0F, 0xB6, 0x89};

			#define DIRECTOR_API_VERSION_1103_TESTED_CODE_SIZE DIRECTOR_API_VERSION_11_TESTED_CODE_SIZE
			#define directorAPIVersion1103TestedCode directorAPIVersion11TestedCode

			#define DIRECTOR_API_VERSION_115_TESTED_CODE_SIZE DIRECTOR_API_VERSION_11_TESTED_CODE_SIZE
			#define directorAPIVersion115TestedCode directorAPIVersion11TestedCode

			const VIRTUAL_SIZE DIRECTOR_API_VERSION_1158_TESTED_CODE_SIZE = 15;
			CODE1 directorAPIVersion1158TestedCode[DIRECTOR_API_VERSION_1158_TESTED_CODE_SIZE] = {0x81, 0xF9, 0xF2, 0x00, 0x00, 0x00, 0x0F, 0x87, 0xE2, 0x1F, 0x00, 0x00, 0x0F, 0xB6, 0x89};

			const VIRTUAL_SIZE DIRECTOR_API_VERSION_1159_TESTED_CODE_SIZE = 14;
			CODE1 directorAPIVersion1159TestedCode[DIRECTOR_API_VERSION_1159_TESTED_CODE_SIZE] = {0x3D, 0xF3, 0x00, 0x00, 0x00, 0x0F, 0x87, 0x8C, 0x20, 0x00, 0x00, 0x0F, 0xB6, 0x88};

			const VIRTUAL_SIZE DIRECTOR_API_VERSION_12_TESTED_CODE_SIZE = 14;
			CODE1 directorAPIVersion12TestedCode[DIRECTOR_API_VERSION_12_TESTED_CODE_SIZE] = {0x3D, 0xF3, 0x00, 0x00, 0x00, 0x0F, 0x87, 0xE7, 0x1F, 0x00, 0x00, 0x0F, 0xB6, 0x88};
			
			const size_t DIRECTOR_API_DIRECTOR_VERSION_TESTS_SIZE = 14;
			ModuleDirectorVersionTest directorAPIVersionTests[DIRECTOR_API_DIRECTOR_VERSION_TESTS_SIZE] = {{0x000EEC9C, DIRECTOR_API_VERSION_8_TESTED_CODE_SIZE, directorAPIVersion8TestedCode}, {0x000C8440, DIRECTOR_API_VERSION_85_TESTED_CODE_SIZE, directorAPIVersion85TestedCode}, {0x000C84C0, DIRECTOR_API_VERSION_851_TESTED_CODE_SIZE, directorAPIVersion851TestedCode}, {0x000BC9A0, DIRECTOR_API_VERSION_9_TESTED_CODE_SIZE, directorAPIVersion9TestedCode}, {0x000D5684, DIRECTOR_API_VERSION_10_TESTED_CODE_SIZE, directorAPIVersion10TestedCode}, {0x000D6A74, DIRECTOR_API_VERSION_101_TESTED_CODE_SIZE, directorAPIVersion101TestedCode}, {0x000D6E24, DIRECTOR_API_VERSION_1011_TESTED_CODE_SIZE, directorAPIVersion1011TestedCode}, {0x0010ED0E, DIRECTOR_API_VERSION_11_TESTED_CODE_SIZE, directorAPIVersion11TestedCode}, {0x0010EC9E, DIRECTOR_API_VERSION_1103_TESTED_CODE_SIZE, directorAPIVersion1103TestedCode}, {0x001118FE, DIRECTOR_API_VERSION_115_TESTED_CODE_SIZE, directorAPIVersion115TestedCode}, {0x0011245E, DIRECTOR_API_VERSION_1158_TESTED_CODE_SIZE, directorAPIVersion1158TestedCode}, {0x001149E3, DIRECTOR_API_VERSION_1159_TESTED_CODE_SIZE, directorAPIVersion1159TestedCode}, {0x00000000, 0, {}}, {0x00034BF8, DIRECTOR_API_VERSION_12_TESTED_CODE_SIZE, directorAPIVersion12TestedCode}};
			
			directorAPIDirectorVersion = getModuleDirectorVersion(moduleHandle, DIRECTOR_API_DIRECTOR_VERSION_TESTS_SIZE, directorAPIVersionTests);
		}

		if (directorAPIDirectorVersion == MODULE_DIRECTOR_INCOMPATIBLE) {
			callLingoAlertIncompatibleDirectorVersion("Failed to Get Module Director Version", moaDrMovieInterfacePointer, moaMmValueInterfacePointer);
			return false;
		}

		// extend the code
		switch (directorAPIDirectorVersion) {
			case MODULE_DIRECTOR_8:
			// populate the Module Handle Written Code Return Addresses, now that they're relevant
			// here is where the real writes happen at the appropriate addresses
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C11D4);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00103F89);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000EEC9F);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000EEC9F);
				codeExtended = extendCode(moduleHandle, 0x0000F43F, setTheMoviePathExtendedCode8);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C1213);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00103F49);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000EEC9F);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000EEC9F);
				codeExtended = extendCode(moduleHandle, 0x0000F319, setTheMovieNameExtendedCode8);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000C56D0);
				codeExtended = extendCode(moduleHandle, 0x000C56C9, setTheEnvironment_shockMachineExtendedCode8);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000C572B);
				codeExtended = extendCode(moduleHandle, 0x000C5720, setTheEnvironment_shockMachineVersionExtendedCode8);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C1D4B);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C5764);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000BE5C4);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000EEC9F);
				codeExtended = extendCode(moduleHandle, 0x00020E55, setThePlatformExtendedCode8);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C1DE1);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000C57C8);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000EEC9F);

				if (!extendCode(moduleHandle, 0x000465B9, setTheRunModeExtendedCode8)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000C57C0, setTheEnvironment_runModeExtendedCode8);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C5956);
				setTheEnvironment_productBuildVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000BE3C0);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000EEC9F);
				codeExtended = extendCode(moduleHandle, 0x0003671F, setTheEnvironment_productBuildVersionExtendedCode8);
				break;
				case m_setTheProductVersion:
				// the environment.productVersion does not exist
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C1DE1);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000EEC9F);
				codeExtended = extendCode(moduleHandle, 0x00046673 - 0x00000010, setTheProductVersionExtendedCode8);
				break;
				case m_setTheEnvironment_osVersion:
				// the environment.osVersion does not exist
				codeExtended = true;
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000EDDA6);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00020E40);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000EEC9F);
				codeExtended = extendCode(moduleHandle, 0x00103E95, setTheMachineTypeExtendedCode8);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000EDD10);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000EDDA6);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0010476B); // causes 0xBAADF00D?
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x001048BC); // causes 0xBAADF00D?
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x001046EB);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x001153E0);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0000F6A0 - 0x00000010);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000BE36C);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x001047A0);

				if (!extendCode(moduleHandle, 0x00104738, setExternalParamNameExtendedCode8)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x00104888, setExternalParamValueExtendedCode8)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x001046C5, setExternalParamCountExtendedCode8);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00014EB3);

				if (!extendCode(moduleHandle, 0x000149C5, forceTheExitLockExtendedCode8)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000098C0, forceTheExitLockExtendedCode238)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000C130, forceTheExitLockExtendedCode238);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0000E650);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000222E0);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0005BB90);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000BE62A);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0002239C);
				codeExtended = extendCode(moduleHandle, 0x00022319, forceTheSafePlayerExtendedCode8);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_85:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A9597);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A88FA);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9E6A);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000CA275);
				codeExtended = extendCode(moduleHandle, 0x0000D16E, setTheMoviePathExtendedCode85);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A95B5);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A88BF);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9E6A);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000CA275);
				codeExtended = extendCode(moduleHandle, 0x0000D073, setTheMovieNameExtendedCode85);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC1A6);
				codeExtended = extendCode(moduleHandle, 0x000AC19A, setTheEnvironment_shockMachineExtendedCode85);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC1F9);
				codeExtended = extendCode(moduleHandle, 0x000AC1EF, setTheEnvironment_shockMachineVersionExtendedCode85);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A9CB3);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC236);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A2B28);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001BA21);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9E6A);
				codeExtended = extendCode(moduleHandle, 0x0001BA18, setThePlatformExtendedCode85);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A9D11);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC2A3);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9E6A);

				if (!extendCode(moduleHandle, 0x0003B8A8, setTheRunModeExtendedCode85)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000AC299, setTheEnvironment_runModeExtendedCode85);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC41A);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9E6A);
				codeExtended = extendCode(moduleHandle, 0x0003B942, setTheEnvironment_productBuildVersionExtendedCode85);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A9D11);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC479);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AA244);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9E6A);

				if (!extendCode(moduleHandle, 0x0003B90C, setTheProductVersionExtendedCode85)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000AC46F, setTheEnvironment_productVersionExtendedCode85);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC236);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A2B28);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9E6A);
				codeExtended = extendCode(moduleHandle, 0x0001BA27, setTheEnvironment_osVersionExtendedCode85);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C74F4);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0001B9FB);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000CA275);
				codeExtended = extendCode(moduleHandle, 0x000A8826, setTheMachineTypeExtendedCode85);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C7422);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C74F4);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000A8F5B); // causes 0xBAADF00D? but don't use 0x000A8F62, or the first parameter will always be selected
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000A906B); // causes 0xBAADF00D? but don't use 0x000A9072, or the first parameter will always be selected
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000A8EF8);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000F3228);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0000D359);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000A28BE);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000B8AA9);

				if (!extendCode(moduleHandle, 0x000A8F29, setExternalParamNameExtendedCode85)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000A9038, setExternalParamValueExtendedCode85)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000A8ECF, setExternalParamCountExtendedCode85);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001152C);

				if (!extendCode(moduleHandle, 0x0001165E, forceTheExitLockExtendedCode85)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x00008462, forceTheExitLockExtendedCode2385)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000A4AB, forceTheExitLockExtendedCode2385);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0000C4B1);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0001CAC5);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0002CD09);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000A2B8E);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001CB1D);
				codeExtended = extendCode(moduleHandle, 0x0001CAEB, forceTheSafePlayerExtendedCode85);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_851:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A95FD);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A8960);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9EEA);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000CA2F5);
				codeExtended = extendCode(moduleHandle, 0x0000D1A2, setTheMoviePathExtendedCode851);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A961B);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A8925);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9EEA);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000CA2F5);
				codeExtended = extendCode(moduleHandle, 0x0000D0A7, setTheMovieNameExtendedCode851);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC21A);
				codeExtended = extendCode(moduleHandle, 0x000AC20E, setTheEnvironment_shockMachineExtendedCode851);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC26D);
				codeExtended = extendCode(moduleHandle, 0x000AC263, setTheEnvironment_shockMachineVersionExtendedCode851);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A9D19);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC2AA);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A2B88);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001BA5A);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9EEA);
				codeExtended = extendCode(moduleHandle, 0x0001BA51, setThePlatformExtendedCode851);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A9D77);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC317);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9EEA);

				if (!extendCode(moduleHandle, 0x0003B8D9, setTheRunModeExtendedCode851)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000AC30D, setTheEnvironment_runModeExtendedCode851);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC48E);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9EEA);
				codeExtended = extendCode(moduleHandle, 0x0003B973, setTheEnvironment_productBuildVersionExtendedCode851);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000A9D77);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC4ED);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AA2AA);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9EEA);

				if (!extendCode(moduleHandle, 0x0003B93D, setTheProductVersionExtendedCode851)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000AC4E3, setTheEnvironment_productVersionExtendedCode851);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AC2AA);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A2B88);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C9EEA);
				codeExtended = extendCode(moduleHandle, 0x0001BA60, setTheEnvironment_osVersionExtendedCode851);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C7574);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0001BA34);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000CA2F5);
				codeExtended = extendCode(moduleHandle, 0x000A888C, setTheMachineTypeExtendedCode851);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C74A2);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000C7574);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000A8FC1); // causes 0xBAADF00D? but don't use 0x000A8F62, or the first parameter will always be selected
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000A90D1); // causes 0xBAADF00D? but don't use 0x000A9072, or the first parameter will always be selected
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000A8F5E);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000F3238);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0000D396);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000A291E);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000B8B46);

				if (!extendCode(moduleHandle, 0x000A8F8F, setExternalParamNameExtendedCode851)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000A909E, setExternalParamValueExtendedCode851)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000A8F35, setExternalParamCountExtendedCode851);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00011579 - 0x00000010);

				if (!extendCode(moduleHandle, 0x000116A1, forceTheExitLockExtendedCode851)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000084A5, forceTheExitLockExtendedCode23851)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000A4E8, forceTheExitLockExtendedCode23851);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0000C4EE);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0001CAFE);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0002CD70);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000A2BEE);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001CB56);
				codeExtended = extendCode(moduleHandle, 0x0001CB24, forceTheSafePlayerExtendedCode851);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_9:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000CDDB7);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000CD11A);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000BE3CA);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000BE7D5);
				codeExtended = extendCode(moduleHandle, 0x0000D191, setTheMoviePathExtendedCode9);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000CDDD5);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000CD0DF);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000BE3CA);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000BE7D5);
				codeExtended = extendCode(moduleHandle, 0x0000D096, setTheMovieNameExtendedCode9);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D09EA);
				codeExtended = extendCode(moduleHandle, 0x000D09DE, setTheEnvironment_shockMachineExtendedCode9);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D0A3D);
				codeExtended = extendCode(moduleHandle, 0x000D0A33, setTheEnvironment_shockMachineVersionExtendedCode9);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000CE4D3);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D0A7A);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A2B08);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001BA48);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000BE3CA);
				codeExtended = extendCode(moduleHandle, 0x0001BA3F, setThePlatformExtendedCode9);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000CE531);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D0AE7);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000BE3CA);

				if (!extendCode(moduleHandle, 0x0003B8E8, setTheRunModeExtendedCode9)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000D0ADD, setTheEnvironment_runModeExtendedCode9);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D0C5E);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000BE3CA);
				codeExtended = extendCode(moduleHandle, 0x0003B982, setTheEnvironment_productBuildVersionExtendedCode9);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000CE531);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D0CBD);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000CEA64);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000BE3CA);

				if (!extendCode(moduleHandle, 0x0003B94C, setTheProductVersionExtendedCode9)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000D0CB3, setTheEnvironment_productVersionExtendedCode9);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D0A7A);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A2B08);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000BE3CA);
				codeExtended = extendCode(moduleHandle, 0x0001BA4E, setTheEnvironment_osVersionExtendedCode9);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000BBA54);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0001BA22);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000BE7D5);
				codeExtended = extendCode(moduleHandle, 0x000CD046, setTheMachineTypeExtendedCode9);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000BB982);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000BBA54);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000CD77B); // causes 0xBAADF00D? but don't use 0x000A8F62, or the first parameter will always be selected
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000CD88B); // causes 0xBAADF00D? but don't use 0x000A9072, or the first parameter will always be selected
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000CD718);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000F3230);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0000D37C);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000A289E);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000A90D0);

				if (!extendCode(moduleHandle, 0x000CD749, setExternalParamNameExtendedCode9)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000CD858, setExternalParamValueExtendedCode9)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000CD6EF, setExternalParamCountExtendedCode9);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001154F);

				if (!extendCode(moduleHandle, 0x00011687, forceTheExitLockExtendedCode9)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x0000847A, forceTheExitLockExtendedCode239)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000A4BE, forceTheExitLockExtendedCode239);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0001CAEC);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0000C4D4);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0002CD30);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000A2B6E);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001CB44);
				codeExtended = extendCode(moduleHandle, 0x0001CB12, forceTheSafePlayerExtendedCode9);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_10:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AE982);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B2E8D);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D7059);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000D7463);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D712F);
				codeExtended = extendCode(moduleHandle, 0x0000D3E9, setTheMoviePathExtendedCode10);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000ADFCD);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B2E52);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D7059);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000D7463);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D712F);
				codeExtended = extendCode(moduleHandle, 0x0000D2F0, setTheMovieNameExtendedCode10);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B18E0);
				codeExtended = extendCode(moduleHandle, 0x000B18D4, setTheEnvironment_shockMachineExtendedCode10);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B1933);
				codeExtended = extendCode(moduleHandle, 0x000B1929, setTheEnvironment_shockMachineVersionExtendedCode10);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AF32F);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B1970);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A6374);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001D6F3);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D7059);
				codeExtended = extendCode(moduleHandle, 0x0001D6EA, setThePlatformExtendedCode10);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AD760);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B19DD);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D7059);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D712F);

				if (!extendCode(moduleHandle, 0x0003E1F7, setTheRunModeExtendedCode10)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000B19D3, setTheEnvironment_runModeExtendedCode10);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B1B54);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D7059);
				codeExtended = extendCode(moduleHandle, 0x0003E2CD, setTheEnvironment_productBuildVersionExtendedCode10);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AD760);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B1BB3);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AF1D0);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D7059);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D712F);

				if (!extendCode(moduleHandle, 0x0003E297, setTheProductVersionExtendedCode10)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000B1BA9, setTheEnvironment_productVersionExtendedCode10);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B1970);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A6374);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D7059);
				codeExtended = extendCode(moduleHandle, 0x0001D6F9, setTheEnvironment_osVersionExtendedCode10);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D472F);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0001D6C1);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000D7463);
				codeExtended = extendCode(moduleHandle, 0x000B2DB9, setTheMachineTypeExtendedCode10);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D465D);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D472F);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B34A9); // causes 0xBAADF00D?
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B35B9); // causes 0xBAADF00D?
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B2667);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0014FD80);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0000D5D2);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000B34C6);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000A60FE);

				if (!extendCode(moduleHandle, 0x000B3477, setExternalParamNameExtendedCode10)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000B3586, setExternalParamValueExtendedCode10)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000B263E, setExternalParamCountExtendedCode10);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00013D34);

				if (!extendCode(moduleHandle, 0x00013E66, forceTheExitLockExtendedCode10)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000084BD, forceTheExitLockExtendedCode2310)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000A629, forceTheExitLockExtendedCode2310);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0000C736);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0001E810);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0002F2DA);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000A63DA);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001E868);
				codeExtended = extendCode(moduleHandle, 0x0001E836, forceTheSafePlayerExtendedCode10);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_101:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AFCE6);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B41FF);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D8449);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000D8853);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D851F);
				codeExtended = extendCode(moduleHandle, 0x0000D4B4, setTheMoviePathExtendedCode101);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AF2F6);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B41C4);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D8449);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000D8853);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D851F);
				codeExtended = extendCode(moduleHandle, 0x0000D3BB, setTheMovieNameExtendedCode101);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B2C33);
				codeExtended = extendCode(moduleHandle, 0x000B2C27, setTheEnvironment_shockMachineExtendedCode101);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B2C86);
				codeExtended = extendCode(moduleHandle, 0x000B2C7C, setTheEnvironment_shockMachineVersionExtendedCode101);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B06A3 - 0x00000010);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B2CC3);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A777A);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001DB0B);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D8449);
				codeExtended = extendCode(moduleHandle, 0x0001DB02, setThePlatformExtendedCode101);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AEA87);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B2D30);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D8449);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D851F);

				if (!extendCode(moduleHandle, 0x0003E846, setTheRunModeExtendedCode101)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000B2D26, setTheEnvironment_runModeExtendedCode101);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B2EA7);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D8449);
				codeExtended = extendCode(moduleHandle, 0x0003E91C, setTheEnvironment_productBuildVersionExtendedCode101);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AEA87);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B2F06);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B0534);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D8449);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D851F);

				if (!extendCode(moduleHandle, 0x0003E8E6, setTheProductVersionExtendedCode101)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000B2EFC, setTheEnvironment_productVersionExtendedCode101);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B2CC3);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A777A);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D8449);
				codeExtended = extendCode(moduleHandle, 0x0001DB11, setTheEnvironment_osVersionExtendedCode101);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D5B1F);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0001DAD9);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000D8853);
				codeExtended = extendCode(moduleHandle, 0x000B412B, setTheMachineTypeExtendedCode101);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D5A4D);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D5B1F);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B481B); // causes 0xBAADF00D?
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B492B); // causes 0xBAADF00D?
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B39D9);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00150EE0);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0000D6AD - 0x00000010);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000A74FE);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000C70D4);

				if (!extendCode(moduleHandle, 0x000B47E9, setExternalParamNameExtendedCode101)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000B48F8, setExternalParamValueExtendedCode101)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000B39B0, setExternalParamCountExtendedCode101);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00013DFF);

				if (!extendCode(moduleHandle, 0x00013F31, forceTheExitLockExtendedCode101)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x00008504, forceTheExitLockExtendedCode23101)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000A68C, forceTheExitLockExtendedCode23101);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0000C801);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0001EC28);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0002F7C1);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000A77E0);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001EC76);
				codeExtended = extendCode(moduleHandle, 0x0001EC4E, forceTheSafePlayerExtendedCode101);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_1011:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B00DD);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B45F6);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D87F9);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000D8C03);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D88CF);
				codeExtended = extendCode(moduleHandle, 0x0000D4DC, setTheMoviePathExtendedCode1011);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AF6ED);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B45BB);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D87F9);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000D8C03);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D88CF);
				codeExtended = extendCode(moduleHandle, 0x0000D3E3, setTheMovieNameExtendedCode1011);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B303F);
				codeExtended = extendCode(moduleHandle, 0x000B3033, setTheEnvironment_shockMachineExtendedCode1011);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B3092);
				codeExtended = extendCode(moduleHandle, 0x000B3088, setTheEnvironment_shockMachineVersionExtendedCode1011);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B0A8A);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B30CF);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A7B7A);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001DB58);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D87F9);
				codeExtended = extendCode(moduleHandle, 0x0001DB4F, setThePlatformExtendedCode1011);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AEE7E);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B313C);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D87F9);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D88CF);

				if (!extendCode(moduleHandle, 0x0003E8F8, setTheRunModeExtendedCode1011)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000B3132, setTheEnvironment_runModeExtendedCode1011);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B32B3);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D87F9);
				codeExtended = extendCode(moduleHandle, 0x0003E9CE, setTheEnvironment_productBuildVersionExtendedCode1011);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000AEE7E);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B3312);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B092B);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D87F9);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000D88CF);

				if (!extendCode(moduleHandle, 0x0003E998, setTheProductVersionExtendedCode1011)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000B3308, setTheEnvironment_productVersionExtendedCode1011);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000B30CF);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000A7B7A);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D87F9);
				codeExtended = extendCode(moduleHandle, 0x0001DB64, setTheEnvironment_osVersionExtendedCode1011);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D5ECF);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0001DB26);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000D8C03);
				codeExtended = extendCode(moduleHandle, 0x000B4522, setTheMachineTypeExtendedCode1011);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D5DFD);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D5ECF);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B4C12); // causes 0xBAADF00D?
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B4D22); // causes 0xBAADF00D?
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000B3DD0);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x001520A0);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0000D6C5);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000A78FE);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000C74CA);

				if (!extendCode(moduleHandle, 0x000B4BE0, setExternalParamNameExtendedCode1011)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000B4CEF, setExternalParamValueExtendedCode1011)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000B3DA7, setExternalParamCountExtendedCode1011);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00013F5F);

				if (!extendCode(moduleHandle, 0x00013F59, forceTheExitLockExtendedCode1011)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x00008517, forceTheExitLockExtendedCode231011)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000A6B4, forceTheExitLockExtendedCode231011);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0000C829);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0001EC75);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0002F83B);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000A7BE0);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001ECC3);
				codeExtended = extendCode(moduleHandle, 0x0001EC9B, forceTheSafePlayerExtendedCode1011);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_11:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DDD45);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DFAB9);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001103ED);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00110805);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001104BB);
				codeExtended = extendCode(moduleHandle, 0x00010979, setTheMoviePathExtendedCode11);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DCDB7);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DFA79);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001103ED);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00110805);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001104BB);
				codeExtended = extendCode(moduleHandle, 0x000108AB, setTheMovieNameExtendedCode11);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9C0A);
				codeExtended = extendCode(moduleHandle, 0x000D9C03, setTheEnvironment_shockMachineExtendedCode11);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9C4C);
				codeExtended = extendCode(moduleHandle, 0x000D9C44, setTheEnvironment_shockMachineVersionExtendedCode11);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DE75D);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9C8A);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000CE022);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0002578D);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001103ED);
				codeExtended = extendCode(moduleHandle, 0x00025786, setThePlatformExtendedCode11);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DBC62);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9CED);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001103ED);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001104BB);

				if (!extendCode(moduleHandle, 0x0005562D, setTheRunModeExtendedCode11)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000D9CE4, setTheEnvironment_runModeExtendedCode11);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9E78);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001103ED);
				codeExtended = extendCode(moduleHandle, 0x0005576E, setTheEnvironment_productBuildVersionExtendedCode11);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DBC62);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9ED3);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DA83A);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001103ED);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001104BB);

				if (!extendCode(moduleHandle, 0x00055724, setTheProductVersionExtendedCode11)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000D9ECA, setTheEnvironment_productVersionExtendedCode11);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9C8A);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000CE022);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001103ED);
				codeExtended = extendCode(moduleHandle, 0x00025798, setTheEnvironment_osVersionExtendedCode11);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0010DCAD);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00025750);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00110805);
				codeExtended = extendCode(moduleHandle, 0x000DF9A0, setTheMachineTypeExtendedCode11);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0010DBF4);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0010DCAD);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E049B); // causes 0xBAADF00D?
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E050C); // causes 0xBAADF00D?
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E032B);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00197890);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00010BC0);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000CDDB2);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000F9AB0);

				if (!extendCode(moduleHandle, 0x000E0468, setExternalParamNameExtendedCode11)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000E04D8, setExternalParamValueExtendedCode11)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000E0305, setExternalParamCountExtendedCode11);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00018C6D);

				if (!extendCode(moduleHandle, 0x000187F8, forceTheExitLockExtendedCode11)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000098D0, forceTheExitLockExtendedCode2311)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000D1A0, forceTheExitLockExtendedCode2311);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0000FC70);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00026DA0);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0003C7F0);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000CE088);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00026E08);
				codeExtended = extendCode(moduleHandle, 0x00026DD0, forceTheSafePlayerExtendedCode11);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_1103:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DDCD5);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DFA49);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0011037D);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00110795);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0011044B);
				codeExtended = extendCode(moduleHandle, 0x000109A9, setTheMoviePathExtendedCode1103);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DCD47);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DFA09);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0011037D);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00110795);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0011044B);
				codeExtended = extendCode(moduleHandle, 0x000108DB, setTheMovieNameExtendedCode1103);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9B9A);
				codeExtended = extendCode(moduleHandle, 0x000D9B93, setTheEnvironment_shockMachineExtendedCode1103);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9BDC);
				codeExtended = extendCode(moduleHandle, 0x000D9BD4, setTheEnvironment_shockMachineVersionExtendedCode1103);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DE6ED);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9C1A);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000CDFB2);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000257BD);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0011037D);
				codeExtended = extendCode(moduleHandle, 0x000257B6, setThePlatformExtendedCode1103);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DBBF2);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9C7D);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0011037D);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0011044B);

				if (!extendCode(moduleHandle, 0x000554DD, setTheRunModeExtendedCode1103)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000D9C74, setTheEnvironment_runModeExtendedCode1103);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9E08);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0011037D);
				codeExtended = extendCode(moduleHandle, 0x0005561E, setTheEnvironment_productBuildVersionExtendedCode1103);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DBBF2);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9E63);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DA7CA);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0011037D);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0011044B);

				if (!extendCode(moduleHandle, 0x000555D4, setTheProductVersionExtendedCode1103)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000D9E5A, setTheEnvironment_productVersionExtendedCode1103);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000D9C1A);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000CDFB2);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0011037D);
				codeExtended = extendCode(moduleHandle, 0x000257C8, setTheEnvironment_osVersionExtendedCode1103);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0010DC3D);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00025780);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00110795);
				codeExtended = extendCode(moduleHandle, 0x000DF930, setTheMachineTypeExtendedCode1103);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0010DB84);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0010DC3D);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E042B); // causes 0xBAADF00D?
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E049C); // causes 0xBAADF00D?
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E02BB);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00197890);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00010BF0);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000CDD42);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000F9A40);

				if (!extendCode(moduleHandle, 0x000E03F8, setExternalParamNameExtendedCode1103)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000E0468, setExternalParamValueExtendedCode1103)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000E0295, setExternalParamCountExtendedCode1103);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00018C9D);

				if (!extendCode(moduleHandle, 0x00018828, forceTheExitLockExtendedCode1103)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000098D0, forceTheExitLockExtendedCode231103)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000D1D0, forceTheExitLockExtendedCode231103);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0000FCA0);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00026DD0);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0003C680);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000CE018);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00026E38);
				codeExtended = extendCode(moduleHandle, 0x00026E00, forceTheSafePlayerExtendedCode1103);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_115:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DFE85);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E1BF9);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00112FDD);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x001133F5);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001130AB);
				codeExtended = extendCode(moduleHandle, 0x00010B19, setTheMoviePathExtendedCode115);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DEEF7);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E1BB9);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00112FDD);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x001133F5);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001130AB);
				codeExtended = extendCode(moduleHandle, 0x00010A4B, setTheMovieNameExtendedCode115);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DBD4A);
				codeExtended = extendCode(moduleHandle, 0x000DBD43, setTheEnvironment_shockMachineExtendedCode115);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DBD8C);
				codeExtended = extendCode(moduleHandle, 0x000DBD84, setTheEnvironment_shockMachineVersionExtendedCode115);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E089D);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DBDCA);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000CE7FE);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000259ED);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00112FDD);
				codeExtended = extendCode(moduleHandle, 0x000259E6, setThePlatformExtendedCode115);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DDDA2);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DBE2D);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00112FDD);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001130AB);

				if (!extendCode(moduleHandle, 0x0005729D, setTheRunModeExtendedCode115)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000DBE24, setTheEnvironment_runModeExtendedCode115);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DBFB8);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00112FDD);
				codeExtended = extendCode(moduleHandle, 0x000573DE, setTheEnvironment_productBuildVersionExtendedCode115);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DDDA2);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DC013);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DC97A);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00112FDD);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001130AB);

				if (!extendCode(moduleHandle, 0x00057394, setTheProductVersionExtendedCode115)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000DC00A, setTheEnvironment_productVersionExtendedCode115);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DBDCA);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000CE7FE);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00112FDD);
				codeExtended = extendCode(moduleHandle, 0x000259F8, setTheEnvironment_osVersionExtendedCode115);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0011089D);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000259B0);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x001133F5);
				codeExtended = extendCode(moduleHandle, 0x000E1AE0, setTheMachineTypeExtendedCode115);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001107E4);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0011089D);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E25DB); // causes 0xBAADF00D?
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E264C); // causes 0xBAADF00D?
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E246B);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00197910);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00010D60);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000CE582);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000FC410);

				if (!extendCode(moduleHandle, 0x000E25A8, setExternalParamNameExtendedCode115)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000E2618, setExternalParamValueExtendedCode115)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000E2445, setExternalParamCountExtendedCode115);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00018E5D);

				if (!extendCode(moduleHandle, 0x000189E2, forceTheExitLockExtendedCode115)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000098B0, forceTheExitLockExtendedCode23115)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000D270, forceTheExitLockExtendedCode23115);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0000FE10);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00027030);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0003D450);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000CE864);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00027098);
				codeExtended = extendCode(moduleHandle, 0x00027060, forceTheSafePlayerExtendedCode115);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_1158:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E0905);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E2779 - 0x00000010);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00113BA7);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00113FC5);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x00113C75);
				codeExtended = extendCode(moduleHandle, 0x00010E19, setTheMoviePathExtendedCode1158);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DF977);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E2729);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00113BA7);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00113FC5);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x00113C75);
				codeExtended = extendCode(moduleHandle, 0x00010D4B, setTheMovieNameExtendedCode1158);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DC7CA);
				codeExtended = extendCode(moduleHandle, 0x000DC7C3, setTheEnvironment_shockMachineExtendedCode1158);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DC80C);
				codeExtended = extendCode(moduleHandle, 0x000DC804, setTheEnvironment_shockMachineVersionExtendedCode1158);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E140D);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DC84A);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000CF27E);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00025DCD);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00113BA7);
				codeExtended = extendCode(moduleHandle, 0x00025DC6, setThePlatformExtendedCode1158);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DE822);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DC8AD);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00113BA7);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x00113C75);

				if (!extendCode(moduleHandle, 0x0005793D, setTheRunModeExtendedCode1158)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000DC8A4, setTheEnvironment_runModeExtendedCode1158);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DCA38);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00113BA7);
				codeExtended = extendCode(moduleHandle, 0x00057A7E, setTheEnvironment_productBuildVersionExtendedCode1158);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DE822);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DCA93);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DD3FA);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00113BA7);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x00113C75);

				if (!extendCode(moduleHandle, 0x00057A34, setTheProductVersionExtendedCode1158)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000DCA8A, setTheEnvironment_productVersionExtendedCode1158);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DC84A);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000CF27E);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00113BA7);
				codeExtended = extendCode(moduleHandle, 0x00025DD8, setTheEnvironment_osVersionExtendedCode1158);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001113DD);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00025D90);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00113FC5);
				codeExtended = extendCode(moduleHandle, 0x000E2650, setTheMachineTypeExtendedCode1158);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00111324);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001113DD);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E314B); // causes 0xBAADF00D?
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E31BC); // causes 0xBAADF00D?
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E2FDB);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00198930);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00011060);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000CF002);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000FE0B0);

				if (!extendCode(moduleHandle, 0x000E3118, setExternalParamNameExtendedCode1158)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000E3188, setExternalParamValueExtendedCode1158)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000E2FB5, setExternalParamCountExtendedCode1158);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000191FD);

				if (!extendCode(moduleHandle, 0x00018D88, forceTheExitLockExtendedCode1158)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000099D0, forceTheExitLockExtendedCode231158)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000D570, forceTheExitLockExtendedCode231158);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00010110);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00027410);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0003DAF0);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000CF2E4);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x00027478);
				codeExtended = extendCode(moduleHandle, 0x00027440, forceTheSafePlayerExtendedCode1158);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_1159:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E1AB5);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E3929);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001161ED);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00116616);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001162BB);
				codeExtended = extendCode(moduleHandle, 0x00010FB9, setTheMoviePathExtendedCode1159);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E0B27);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E38E9);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001161ED);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00116616);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001162BB);
				codeExtended = extendCode(moduleHandle, 0x00010EEB, setTheMovieNameExtendedCode1159);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DD84A);
				codeExtended = extendCode(moduleHandle, 0x000DD843, setTheEnvironment_shockMachineExtendedCode1159);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DD88C);
				codeExtended = extendCode(moduleHandle, 0x000DD884, setTheEnvironment_shockMachineVersionExtendedCode1159);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000E25BD);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DD8CA);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000CFFF4);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0002601D);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001161ED);
				codeExtended = extendCode(moduleHandle, 0x00026016, setThePlatformExtendedCode1159);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DF9D2);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DD92D);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001161ED);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001162BB);

				if (!extendCode(moduleHandle, 0x00057D6D, setTheRunModeExtendedCode1159)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000DD924, setTheEnvironment_runModeExtendedCode1159);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DDAB8);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001161ED);
				codeExtended = extendCode(moduleHandle, 0x00057EAE, setTheEnvironment_productBuildVersionExtendedCode1159);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DF9D2);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DDB13);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DE47A);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001161ED);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001162BB);

				if (!extendCode(moduleHandle, 0x00057E64, setTheProductVersionExtendedCode1159)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000DDB0A, setTheEnvironment_productVersionExtendedCode1159);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000DD8CA);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000CFFF4);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001161ED);
				codeExtended = extendCode(moduleHandle, 0x00026028, setTheEnvironment_osVersionExtendedCode1159);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001136E2);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00025FE0);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00116616);
				codeExtended = extendCode(moduleHandle, 0x000E3810, setTheMachineTypeExtendedCode1159);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0011361C);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x001136E2);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E43BB); // causes 0xBAADF00D?
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E442C); // causes 0xBAADF00D?
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000E422B);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0019B930);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00011200);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000CFD72);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000FF9F0);

				if (!extendCode(moduleHandle, 0x000E4388, setExternalParamNameExtendedCode1159)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x000E43F8, setExternalParamValueExtendedCode1159)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x000E4215 - 0x00000010, setExternalParamCountExtendedCode1159);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000193AD);

				if (!extendCode(moduleHandle, 0x00018F38, forceTheExitLockExtendedCode1159)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x00009B20, forceTheExitLockExtendedCode231159)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000D710, forceTheExitLockExtendedCode231159);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x000102B0);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x00027660);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0003DDA0);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x000D005A);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000276C8);
				codeExtended = extendCode(moduleHandle, 0x000276A0 - 0x00000010, forceTheSafePlayerExtendedCode1159);
				
				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
			break;
			case MODULE_DIRECTOR_12:
			switch (methodSelector) {
				case m_setTheMoviePath:
				setTheMoviePathExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00013CB0);
				setThePathNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00018D64);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000363FC);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000367C8);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000364BC);
				exceptionHandlerSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00001762);
				codeExtended = extendCode(moduleHandle, 0x000621F7, setTheMoviePathExtendedCode12);
				break;
				case m_setTheMovieName:
				setTheMovieNameExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00012DA0);
				setTheMovieExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00018D04);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000363FC);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000367C8);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000364BC);
				exceptionHandlerSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00001762);
				codeExtended = extendCode(moduleHandle, 0x00062104, setTheMovieNameExtendedCode12);
				break;
				case m_setTheEnvironment_shockMachine:
				setTheEnvironment_shockMachineExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0000FB4B);
				codeExtended = extendCode(moduleHandle, 0x0000FB42, setTheEnvironment_shockMachineExtendedCode12);
				break;
				case m_setTheEnvironment_shockMachineVersion:
				setTheEnvironment_shockMachineVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0000FB95);
				codeExtended = extendCode(moduleHandle, 0x0000FB8E, setTheEnvironment_shockMachineVersionExtendedCode12);
				break;
				case m_setThePlatform:
				setThePlatformExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000146A7);
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0000FBDC);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00166A56);
				setThePlatformExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0007AC1E);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000363FC);
				codeExtended = extendCode(moduleHandle, 0x0007AC17, setThePlatformExtendedCode12);
				break;
				case m_setTheRunMode:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000121B9);
				setTheEnvironment_runModeExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0000FC53);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000363FC);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000364BC);
				exceptionHandlerSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00001762);

				if (!extendCode(moduleHandle, 0x000A3153, setTheRunModeExtendedCode12)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000FC46, setTheEnvironment_runModeExtendedCode12);
				break;
				case m_setTheEnvironment_productBuildVersion:
				setTheEnvironment_productBuildVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0000FE22);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000363FC);
				exceptionHandlerSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00001762);
				codeExtended = extendCode(moduleHandle, 0x000A3232, setTheEnvironment_productBuildVersionExtendedCode12);
				break;
				case m_setTheProductVersion:
				setTheRunModeTheProductVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000121B9);
				setTheEnvironment_productVersionExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0000FE8F);
				theEnvironmentCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00010AA5);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000363FC);
				lingoCallCompareAddress3 = makeExtendedCodeAddress(moduleHandle, 0x000364BC);
				exceptionHandlerSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00001762);

				if (!extendCode(moduleHandle, 0x000A3204, setTheProductVersionExtendedCode12)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0000FE82, setTheEnvironment_productVersionExtendedCode12);
				break;
				case m_setTheEnvironment_osVersion:
				setTheEnvironment_platformTheEnvironment_osVersionExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x0000FBDC);
				setThePlatformTheEnvironment_osVersionExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00166A56);
				lingoCallCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000363FC);
				exceptionHandlerSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00001762);
				codeExtended = extendCode(moduleHandle, 0x0007AC22, setTheEnvironment_osVersionExtendedCode12);
				break;
				case m_setTheMachineType:
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00033898);
				setTheMachineTypeExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x0007ABD0);
				lingoCallCompareAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000367C8);
				codeExtended = extendCode(moduleHandle, 0x00018C34, setTheMachineTypeExtendedCode12);
				break;
				case m_setExternalParam:
				setExternalParamNameExternalParamValueExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x000337BC);
				setTheMachineTypeExternalParamCountExtendedCodeCompareAddress = makeExtendedCodeAddress(moduleHandle, 0x00033898);
				setExternalParamNameExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000198EB); // causes 0xBAADF00D?
				setExternalParamValueExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001995C); // causes 0xBAADF00D?
				setExternalParamCountExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0001974C);
				setExternalParamExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x001962C8);
				setExternalParamExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x000624A0);
				setExternalParamExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x001668B2);
				setExternalParamExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x00033170);

				if (!extendCode(moduleHandle, 0x000198B3, setExternalParamNameExtendedCode12)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x00019923, setExternalParamValueExtendedCode12)) {
					goto error;
				}

				// we have to update the count of External Params too
				codeExtended = extendCode(moduleHandle, 0x00019723, setExternalParamCountExtendedCode12);
				break;
				case m_forceTheExitLock:
				forceTheExitLockExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0006EFD8);

				if (!extendCode(moduleHandle, 0x0006EB81, forceTheExitLockExtendedCode12)) {
					goto error;
				}

				if (!extendCode(moduleHandle, 0x00009430 - 0x00000010, forceTheExitLockExtendedCode212)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x0005E590, forceTheExitLockExtendedCode312);
				break;
				case m_forceTheSafePlayer:
				forceTheSafePlayerExtendedCodeSubroutineAddress = makeExtendedCodeAddress(moduleHandle, 0x00061380);
				forceTheSafePlayerExtendedCodeSubroutineAddress2 = makeExtendedCodeAddress(moduleHandle, 0x0007C170);
				forceTheSafePlayerExtendedCodeSubroutineAddress3 = makeExtendedCodeAddress(moduleHandle, 0x0008F380);
				forceTheSafePlayerExtendedCodeSubroutineAddress4 = makeExtendedCodeAddress(moduleHandle, 0x00166E4C);
				forceTheSafePlayerExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x0007C1D7);
				codeExtended = extendCode(moduleHandle, 0x0007C193, forceTheSafePlayerExtendedCode12);

				if (!setLingoSafePlayer(theSafePlayer, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
					goto noSafePlayer;
				}
			}
		}
		break;
		case MODULE_NET_LINGO_XTRA:
		// much simpler than the Director API, there are only two very similar things you can do here
		// test the code
		{
			const VIRTUAL_SIZE NET_LINGO_XTRA_DIRECTOR_VERSION_8_TESTED_CODE_SIZE = 12;
			CODE1 netLingoXtraDirectorVersion8TestedCode[NET_LINGO_XTRA_DIRECTOR_VERSION_8_TESTED_CODE_SIZE] = {0x83, 0xFA, 0x17, 0x0F, 0x87, 0x54, 0x01, 0x00, 0x00, 0xFF, 0x24, 0x95};

			const VIRTUAL_SIZE NET_LINGO_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE = 12;
			CODE1 netLingoXtraDirectorVersion85TestedCode[NET_LINGO_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE] = {0x83, 0xFA, 0x17, 0x0F, 0x87, 0x31, 0x01, 0x00, 0x00, 0xFF, 0x24, 0x95};

			// NetLingo Xtra Director Version 8.5.1 Test Code is equivalent to NetLingo Xtra Director Version 8.5 Test Code

			#define NET_LINGO_XTRA_DIRECTOR_VERSION_9_TESTED_CODE_SIZE NET_LINGO_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE
			#define netLingoXtraDirectorVersion9TestedCode netLingoXtraDirectorVersion85TestedCode

			const VIRTUAL_SIZE NET_LINGO_XTRA_DIRECTOR_VERSION_10_TESTED_CODE_SIZE = 12;
			CODE1 netLingoXtraDirectorVersion10TestedCode[NET_LINGO_XTRA_DIRECTOR_VERSION_10_TESTED_CODE_SIZE] = {0x83, 0xFA, 0x17, 0x0F, 0x87, 0x31, 0x01, 0x00, 0x00, 0xFF, 0x24, 0x95};

			#define NET_LINGO_XTRA_DIRECTOR_VERSION_101_TESTED_CODE_SIZE NET_LINGO_XTRA_DIRECTOR_VERSION_10_TESTED_CODE_SIZE
			#define netLingoXtraDirectorVersion101TestedCode netLingoXtraDirectorVersion10TestedCode

			#define NET_LINGO_XTRA_DIRECTOR_VERSION_11_TESTED_CODE_SIZE NET_LINGO_XTRA_DIRECTOR_VERSION_10_TESTED_CODE_SIZE
			#define netLingoXtraDirectorVersion11TestedCode netLingoXtraDirectorVersion10TestedCode

			#define NET_LINGO_XTRA_DIRECTOR_VERSION_1103_TESTED_CODE_SIZE NET_LINGO_XTRA_DIRECTOR_VERSION_10_TESTED_CODE_SIZE
			#define netLingoXtraDirectorVersion1103TestedCode netLingoXtraDirectorVersion10TestedCode

			const VIRTUAL_SIZE NET_LINGO_XTRA_DIRECTOR_VERSION_115_TESTED_CODE_SIZE = 12;
			CODE1 netLingoXtraDirectorVersion115TestedCode[NET_LINGO_XTRA_DIRECTOR_VERSION_115_TESTED_CODE_SIZE] = {0x83, 0xFA, 0x1A, 0x0F, 0x87, 0x5B, 0x01, 0x00, 0x00, 0xFF, 0x24, 0x95};

			#define NET_LINGO_XTRA_DIRECTOR_VERSION_1158_TESTED_CODE_SIZE NET_LINGO_XTRA_DIRECTOR_VERSION_115_TESTED_CODE_SIZE
			#define netLingoXtraDirectorVersion1158TestedCode netLingoXtraDirectorVersion115TestedCode

			#define NET_LINGO_XTRA_DIRECTOR_VERSION_12_TESTED_CODE_SIZE NET_LINGO_XTRA_DIRECTOR_VERSION_115_TESTED_CODE_SIZE
			#define netLingoXtraDirectorVersion12TestedCode netLingoXtraDirectorVersion115TestedCode

			// NetLingo Xtra Director Version 11.5.9 Test Code is equivalent to NetLingo Xtra Director Version 11.5.8 Test Code

			const size_t NET_LINGO_XTRA_DIRECTOR_VERSION_TESTS_SIZE = 14;
			ModuleDirectorVersionTest netLingoXtraDirectorVersionTests[NET_LINGO_XTRA_DIRECTOR_VERSION_TESTS_SIZE] = {{0x00002F89, NET_LINGO_XTRA_DIRECTOR_VERSION_8_TESTED_CODE_SIZE, netLingoXtraDirectorVersion8TestedCode}, {0x0000277A, NET_LINGO_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE, netLingoXtraDirectorVersion85TestedCode}, {0x00000000, 0, {}}, {0x0000274B, NET_LINGO_XTRA_DIRECTOR_VERSION_9_TESTED_CODE_SIZE, netLingoXtraDirectorVersion9TestedCode}, {0x0000277D, NET_LINGO_XTRA_DIRECTOR_VERSION_10_TESTED_CODE_SIZE, netLingoXtraDirectorVersion10TestedCode}, {0x0000277A, NET_LINGO_XTRA_DIRECTOR_VERSION_101_TESTED_CODE_SIZE, netLingoXtraDirectorVersion101TestedCode}, {0x00000000, 0, {}}, {0x0000435A, NET_LINGO_XTRA_DIRECTOR_VERSION_11_TESTED_CODE_SIZE, netLingoXtraDirectorVersion11TestedCode}, {0x0000435A, NET_LINGO_XTRA_DIRECTOR_VERSION_1103_TESTED_CODE_SIZE, netLingoXtraDirectorVersion1103TestedCode}, {0x000043F6, NET_LINGO_XTRA_DIRECTOR_VERSION_115_TESTED_CODE_SIZE, netLingoXtraDirectorVersion115TestedCode}, {0x00004397, NET_LINGO_XTRA_DIRECTOR_VERSION_1158_TESTED_CODE_SIZE, netLingoXtraDirectorVersion1158TestedCode}, {0x00000000, 0, {}}, {0x00000000, 0, {}}, {0x000046CF, NET_LINGO_XTRA_DIRECTOR_VERSION_12_TESTED_CODE_SIZE, netLingoXtraDirectorVersion12TestedCode}};
			
			netLingoXtraDirectorVersion = getModuleDirectorVersion(moduleHandle, NET_LINGO_XTRA_DIRECTOR_VERSION_TESTS_SIZE, netLingoXtraDirectorVersionTests);
		}

		if (netLingoXtraDirectorVersion == MODULE_DIRECTOR_INCOMPATIBLE) {
			callLingoAlertIncompatibleDirectorVersion("Failed to Get Module Director Version", moaDrMovieInterfacePointer, moaMmValueInterfacePointer);
			return false;
		}

		// blanking them out is the same - only the address differs
		// extend the code
		switch (netLingoXtraDirectorVersion) {
			case MODULE_DIRECTOR_8:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				codeExtended = extendCode(moduleHandle, 0x00003310, disableGoToNetThingExtendedCode);
				break;
				case m_disableGoToNetPage:
				codeExtended = extendCode(moduleHandle, 0x000033B0, disableGoToNetThingExtendedCode);
			}
			break;
			case MODULE_DIRECTOR_85:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				codeExtended = extendCode(moduleHandle, 0x00002AA1, disableGoToNetThingExtendedCode);
				break;
				case m_disableGoToNetPage:
				codeExtended = extendCode(moduleHandle, 0x00002B2D, disableGoToNetThingExtendedCode);
			}
			break;
			case MODULE_DIRECTOR_9:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				codeExtended = extendCode(moduleHandle, 0x00002A72, disableGoToNetThingExtendedCode);
				break;
				case m_disableGoToNetPage:
				codeExtended = extendCode(moduleHandle, 0x00002AFE, disableGoToNetThingExtendedCode);
			}
			break;
			case MODULE_DIRECTOR_10:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				codeExtended = extendCode(moduleHandle, 0x00002AA4, disableGoToNetThingExtendedCode);
				break;
				case m_disableGoToNetPage:
				codeExtended = extendCode(moduleHandle, 0x00002B30, disableGoToNetThingExtendedCode);
			}
			break;
			case MODULE_DIRECTOR_101:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				codeExtended = extendCode(moduleHandle, 0x00002AA1, disableGoToNetThingExtendedCode);
				break;
				case m_disableGoToNetPage:
				codeExtended = extendCode(moduleHandle, 0x00002B2D, disableGoToNetThingExtendedCode);
			}
			break;
			case MODULE_DIRECTOR_11:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				codeExtended = extendCode(moduleHandle, 0x0000294F, disableGoToNetThingExtendedCode);
				break;
				case m_disableGoToNetPage:
				codeExtended = extendCode(moduleHandle, 0x00003077, disableGoToNetThingExtendedCode);
			}
			break;
			case MODULE_DIRECTOR_115:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				codeExtended = extendCode(moduleHandle, 0x00002968, disableGoToNetThingExtendedCode);
				break;
				case m_disableGoToNetPage:
				codeExtended = extendCode(moduleHandle, 0x00003090, disableGoToNetThingExtendedCode);
			}
			break;
			case MODULE_DIRECTOR_1158:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				codeExtended = extendCode(moduleHandle, 0x00002909, disableGoToNetThingExtendedCode);
				break;
				case m_disableGoToNetPage:
				codeExtended = extendCode(moduleHandle, 0x00003031, disableGoToNetThingExtendedCode);
			}
			break;
			case MODULE_DIRECTOR_12:
			switch (methodSelector) {
				case m_disableGoToNetMovie:
				codeExtended = extendCode(moduleHandle, 0x00002981, disableGoToNetThingExtendedCode);
				break;
				case m_disableGoToNetPage:
				codeExtended = extendCode(moduleHandle, 0x0000329B, disableGoToNetThingExtendedCode);
			}
		}
		break;
		case MODULE_SHOCKWAVE_3D_ASSET_XTRA:
		// test the code
		{
			const VIRTUAL_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE = 6;
			CODE1 shockwave3DAssetXtraDirectorVersion85TestedCode[SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE] = {0x49, 0x74, 0x14, 0x49, 0x75, 0x1F};

			#define SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_851_TESTED_CODE_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE
			#define shockwave3DAssetXtraDirectorVersion851TestedCode shockwave3DAssetXtraDirectorVersion85TestedCode

			#define SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_9_TESTED_CODE_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE
			#define shockwave3DAssetXtraDirectorVersion9TestedCode shockwave3DAssetXtraDirectorVersion85TestedCode

			#define SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_10_TESTED_CODE_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE
			#define shockwave3DAssetXtraDirectorVersion10TestedCode shockwave3DAssetXtraDirectorVersion85TestedCode

			#define SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_101_TESTED_CODE_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE
			#define shockwave3DAssetXtraDirectorVersion101TestedCode shockwave3DAssetXtraDirectorVersion85TestedCode

			const VIRTUAL_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_11_TESTED_CODE_SIZE = 6;
			CODE1 shockwave3DAssetXtraDirectorVersion11TestedCode[SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_11_TESTED_CODE_SIZE] = {0x49, 0x74, 0x0F, 0x49, 0x75, 0x15};

			#define SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_1103_TESTED_CODE_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_11_TESTED_CODE_SIZE
			#define shockwave3DAssetXtraDirectorVersion1103TestedCode shockwave3DAssetXtraDirectorVersion11TestedCode

			#define SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_115_TESTED_CODE_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_11_TESTED_CODE_SIZE
			#define shockwave3DAssetXtraDirectorVersion115TestedCode shockwave3DAssetXtraDirectorVersion11TestedCode

			#define SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_1158_TESTED_CODE_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_11_TESTED_CODE_SIZE
			#define shockwave3DAssetXtraDirectorVersion1158TestedCode shockwave3DAssetXtraDirectorVersion11TestedCode

			const VIRTUAL_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_1159_TESTED_CODE_SIZE = 9;
			CODE1 shockwave3DAssetXtraDirectorVersion1159TestedCode[SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_1159_TESTED_CODE_SIZE] = {0x49, 0x74, 0x1E, 0x49, 0x74, 0x0F, 0x49, 0x75, 0x21};

			const VIRTUAL_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_1165_TESTED_CODE_SIZE = 15;
			CODE1 shockwave3DAssetXtraDirectorVersion1165TestedCode[SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_1165_TESTED_CODE_SIZE] = {0x83, 0xE9, 0x01, 0x74, 0x22, 0x83, 0xE9, 0x01, 0x74, 0x11, 0x83, 0xE9, 0x01, 0x75, 0x21};

			const VIRTUAL_SIZE SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_12_TESTED_CODE_SIZE = 8;
			CODE1 shockwave3DAssetXtraDirectorVersion12TestedCode[SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_12_TESTED_CODE_SIZE] = {0x83, 0xFA, 0x03, 0x77, 0x31, 0xFF, 0x24, 0x95};

			const size_t SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_TESTS_SIZE = 14;
			ModuleDirectorVersionTest shockwave3DAssetXtraDirectorVersionTests[SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_TESTS_SIZE] = {{0x00000000, 0, {}}, {0x00004DD9, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_85_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion85TestedCode}, {0x00004F39, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_851_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion851TestedCode}, {0x00004EB9, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_9_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion9TestedCode}, {0x00008FA9, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_10_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion10TestedCode}, {0x00008F99, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_101_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion101TestedCode}, {0x00000000, 0, {}}, {0x00004609, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_11_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion11TestedCode}, {0x00004619, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_1103_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion1103TestedCode}, {0x000057E9, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_115_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion115TestedCode}, {0x00005829, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_1158_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion1158TestedCode}, {0x00006A09, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_1159_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion1159TestedCode}, {0x00006DE9, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_1165_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion1165TestedCode}, {0x000073AC, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_12_TESTED_CODE_SIZE, shockwave3DAssetXtraDirectorVersion12TestedCode}};
			
			shockwave3DAssetXtraDirectorVersion = getModuleDirectorVersion(moduleHandle, SHOCKWAVE_3D_ASSET_XTRA_DIRECTOR_VERSION_TESTS_SIZE, shockwave3DAssetXtraDirectorVersionTests);
		}

		if (shockwave3DAssetXtraDirectorVersion == MODULE_DIRECTOR_INCOMPATIBLE) {
			callLingoAlertIncompatibleDirectorVersion("Failed to Get Module Director Version", moaDrMovieInterfacePointer, moaMmValueInterfacePointer);
			return false;
		}

		// extend the code
		switch (shockwave3DAssetXtraDirectorVersion) {
			case MODULE_DIRECTOR_85:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DD5D9);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DD5E7);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000DD5D4, bugfixShockwave3DBadDriverListExtendedCode85)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000DD5E2, bugfixShockwave3DBadDriverListExtendedCode285);
			}
			break;
			case MODULE_DIRECTOR_851:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DE337);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DE345);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000DE332, bugfixShockwave3DBadDriverListExtendedCode851)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000DE340, bugfixShockwave3DBadDriverListExtendedCode2851);
			}
			break;
			case MODULE_DIRECTOR_9:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DAD74);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000DAD82);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000DAD6F, bugfixShockwave3DBadDriverListExtendedCode9)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000DAD7D, bugfixShockwave3DBadDriverListExtendedCode29);
			}
			break;
			case MODULE_DIRECTOR_10:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000FA16A);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000FA178);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000FA165, bugfixShockwave3DBadDriverListExtendedCode10)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000FA173, bugfixShockwave3DBadDriverListExtendedCode210);
			}
			break;
			case MODULE_DIRECTOR_101:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000FA3EC);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000FA3FA);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000FA3E7, bugfixShockwave3DBadDriverListExtendedCode101)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000FA3F5, bugfixShockwave3DBadDriverListExtendedCode2101);
			}
			break;
			case MODULE_DIRECTOR_11:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D3226);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D3234);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000D3221, bugfixShockwave3DBadDriverListExtendedCode11)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000D322F, bugfixShockwave3DBadDriverListExtendedCode211);
			}
			break;
			case MODULE_DIRECTOR_1103:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D321F);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D322D);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000D321A, bugfixShockwave3DBadDriverListExtendedCode1103)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000D3228, bugfixShockwave3DBadDriverListExtendedCode21103);
			}
			break;
			case MODULE_DIRECTOR_115:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D7AFD);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D7B0B);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000D7AF8, bugfixShockwave3DBadDriverListExtendedCode115)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000D7B06, bugfixShockwave3DBadDriverListExtendedCode2115);
			}
			break;
			case MODULE_DIRECTOR_1158:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D8922);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D89DD);
			bugfixShockwave3DBadDriverList3ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000D893B);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000D8915, bugfixShockwave3DBadDriverListExtendedCode1158)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000D8936, bugfixShockwave3DBadDriverListExtendedCode21158);
			}
			break;
			case MODULE_DIRECTOR_1159:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000EB19B);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000EB256);
			bugfixShockwave3DBadDriverList3ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000EB1B4);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000EB18E, bugfixShockwave3DBadDriverListExtendedCode1159)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000EB1AF, bugfixShockwave3DBadDriverListExtendedCode21159);
			}
			break;
			case MODULE_DIRECTOR_1165:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000F3D06);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000F3DB3);
			bugfixShockwave3DBadDriverList3ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000F3D1F);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000F3CF9, bugfixShockwave3DBadDriverListExtendedCode1165)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000F3D1A, bugfixShockwave3DBadDriverListExtendedCode21165);
			}
			break;
			case MODULE_DIRECTOR_12:
			bugfixShockwave3DBadDriverListExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000FF91E);
			bugfixShockwave3DBadDriverList2ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000FFA04);
			bugfixShockwave3DBadDriverList3ExtendedCodeReturnAddress = makeExtendedCodeAddress(moduleHandle, 0x000FF937);

			switch (methodSelector) {
				case m_bugfixShockwave3DBadDriverList:
				if (!extendCode(moduleHandle, 0x000FF90E, bugfixShockwave3DBadDriverListExtendedCode12)) {
					goto error;
				}

				codeExtended = extendCode(moduleHandle, 0x000FF932, bugfixShockwave3DBadDriverListExtendedCode212);
			}
		}
	}

	// in case we failed and didn't catch it somehow
	if (!codeExtended) {
		error:
		callLingoAlertAntivirus("Failed to Extend Code", moaDrMovieInterfacePointer, moaMmValueInterfacePointer);
		return false;
	}
	return true;
	noSafePlayer:
	callLingoAlert("Failed to Set Lingo Safe Player", moaDrMovieInterfacePointer, moaMmValueInterfacePointer);
	return false;
}
/* End Extender */




// "For with God nothing will be impossible." - Luke 1:37