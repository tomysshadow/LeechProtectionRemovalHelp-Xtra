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

#include "shared.h"

#include "moastdif.h"
#include "mmixscrp.h"
#include "driservc.h"
#include "xmmvalue.h"

#include <windows.h>




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
	MODULE_DIRECTOR_1165,
	MODULE_DIRECTOR_12
};




// {EF754E9F-BE76-42D3-A7B2-5BB7947B5812}
DEFINE_GUID(CLSID_TStdXtra,
			0xef754e9f, 0xbe76, 0x42d3, 0xa7, 0xb2, 0x5b, 0xb7, 0x94, 0x7b, 0x58, 0x12);

DEFINE_GUID(CLSID_CNetLingo,
	0x5ace8e76, 0x8011, 0x11d0, 0xb7, 0x87, 0x0, 0x5, 0x2, 0x18, 0x3e, 0x13);

DEFINE_GUID(CLSID_CShockwave3DAsset,
	0xf8bf17e1, 0x88d4, 0x11d3, 0x98, 0x9c, 0x0, 0xa0, 0xc9, 0x90, 0x2d, 0x25);

EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(TStdXtra)
	PIMoaDrPlayer moaDrPlayerInterfacePointer = NULL;
	PIMoaMmValue moaMmValueInterfacePointer = NULL;
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




inline bool callLingoQuit(PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	// we can't just use Win32 API's TerminateProcess because it causes a long pause before closing the movie improperly
	// so we use Lingo's Quit handler
	if (!moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	MoaMmSymbol quitSymbol = 0;
	MoaError err = moaMmValueInterfacePointer->StringToSymbol("Quit", &quitSymbol);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	err = moaDrMovieInterfacePointer->CallHandler(quitSymbol, 0, NULL, NULL);

	if (err != kMoaErr_NoErr) {
		return false;
	}
	return true;
}

inline bool callLingoAlert(ConstPMoaChar messageString, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	// we can't just use Win32 API's MessageBox or it'll cause graphical glitches from the movie not being paused
	// so we use Lingo's Alert handler (as per the XDK's recommendation)
	if (!messageString || !moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	MoaMmSymbol alertSymbol = 0;
	MoaError err = moaMmValueInterfacePointer->StringToSymbol("Alert", &alertSymbol);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	MoaMmValue messageValue = kVoidMoaMmValueInitializer;
	err = moaMmValueInterfacePointer->StringToValue(messageString, &messageValue);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	bool result = false;
	err = moaDrMovieInterfacePointer->CallHandler(alertSymbol, 1, &messageValue, NULL);

	if (err != kMoaErr_NoErr) {
		goto error;
	}
	result = true;
	error:
	err = moaMmValueInterfacePointer->ValueRelease(&messageValue);

	if (err != kMoaErr_NoErr) {
		result = false;
	}
	return result;
}

inline bool callLingoAlertXtraMissing(ConstPMoaChar messageString, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	if (!messageString || !moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	if (!callLingoAlert(messageString, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}

	if (!callLingoAlert("An Xtra that the LeechProtectionRemovalHelp Xtra requires, such as the NetLingo Xtra or Shockwave 3D Asset Xtra, is missing.", moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}
	return true;
}

inline bool callLingoAlertIncompatibleDirectorVersion(ConstPMoaChar messageString, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	if (!messageString || !moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	if (!callLingoAlert(messageString, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}

	if (!callLingoAlert("The LeechProtectionRemovalHelp Xtra has determined it is incompatible with this Director version.", moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}
	return true;
}

inline bool callLingoAlertAntivirus(ConstPMoaChar messageString, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	if (!messageString || !moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	if (!callLingoAlert(messageString, moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}

	if (!callLingoAlert("Your antivirus may be preventing the LeechProtectionRemovalHelp Xtra from working properly.", moaDrMovieInterfacePointer, moaMmValueInterfacePointer)) {
		return false;
	}
	return true;
}

inline bool setLingoSafePlayer(MoaLong safePlayer, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer) {
	if (!moaDrMovieInterfacePointer || !moaMmValueInterfacePointer) {
		return false;
	}

	MoaMmSymbol safePlayerSymbol = 0;
	MoaError err = moaMmValueInterfacePointer->StringToSymbol("SafePlayer", &safePlayerSymbol);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	MoaMmValue safePlayerValue = kVoidMoaMmValueInitializer;
	err = moaMmValueInterfacePointer->IntegerToValue(safePlayer, &safePlayerValue);

	if (err != kMoaErr_NoErr) {
		return false;
	}

	bool result = false;
	err = moaDrMovieInterfacePointer->SetProp(safePlayerSymbol, &safePlayerValue);

	if (err != kMoaErr_NoErr) {
		goto error;
	}

	result = true;
	error:
	err = moaMmValueInterfacePointer->ValueRelease(&safePlayerValue);

	if (err != kMoaErr_NoErr) {
		result = false;
	}
	return result;
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




bool extender(MoaMmSymbol methodSelector, MODULE module, PIMoaDrMovie moaDrMovieInterfacePointer, PIMoaMmValue moaMmValueInterfacePointer, PIMoaCallback callbackPointer);

#endif