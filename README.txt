LeechProtectionRemovalHelp Xtra 1.3.4
By Anthony Kleine

	This Director Xtra allows for the archival of Shockwave Movies
	by setting, forcing, disabling or bugfixing properties
	which will stay set, forced, disabled or bugfixed even
	after going to other movies.

	The LeechProtectionRemovalHelp Xtra has been tested with
	a number of Director versions from 8 - 12.

	This Xtra is NOT Shockwave Safe.


Video Tutorial
	Coming Soon


Tested With
	-8.0.0.178
	-8.5.0.321
	-8.5.1.102
	-9.0.0.383
	-10.0.0.188
	-10.1.0.11
	-11.0.0.426
	-11.0.3.466
	-11.5.0.593
	-11.5.8.612
	-11.5.9.629
	-12.0.0.111

	Compatibility with untested versions is not guaranteed.


Usage
	on exitFrame me
		setTheMoviePath("http://addictinggames.com/newGames/metalmayhemworldtour/")
		setTheMovieName("metalmayhemworldtour.dcr")
		setTheEnvironment_shockMachine(1)
		setTheEnvironment_shockMachineVersion("")
		setThePlatform("Windows,32")
		setTheRunMode("Author")
		setTheEnvironment_productBuildVersion("593")
		setTheProductVersion("11.5")
		setTheEnvironment_osVersion("Windows,6,2,148,2,")
		setTheMachineType(256)
		forceTheExitLock(0)
		forceTheSafePlayer(0)
		setExternalParam("src", "/newGames/metalmayhemworldtour/metalmayhemworldtour.dcr")
		setExternalParam("sw2", "121220")
		disableGoToNetMovie()
		disableGoToNetPage()
		bugfixShockwave3DBadDriverList()
		go(1, "metalmayhemworldtour.dcr")
	end


Message Table
	-- Set Properties
	* setTheMoviePath string moviePath -- sets the moviePath (and the path, and the pathName) property (does not work while debugging a Lingo Script)
	* setTheMovieName string movieName -- sets the movieName (and the movie) property (does not work while debugging a Lingo Script)
	* setTheEnvironment_shockMachine int environment_shockMachine -- sets the environment.shockMachine (and the environmentPropList.shockMachine) property (does not work while debugging a Lingo Script)
	* setTheEnvironment_shockMachineVersion string environment_shockMachineVersion -- sets the environment.shockMachineVersion (and the environmentPropList.shockMachineVersion) property (does not work while debugging a Lingo Script)
	* setThePlatform string platform -- sets the platform (and the environment.platform, and the environmentPropList.platform) property (does not work while debugging a Lingo Script)
	* setTheRunMode string runMode -- sets the runMode (and the environment.runMode, and the environmentPropList.runMode) property (does not work while debugging a Lingo Script)
	* setTheEnvironment_productBuildVersion string environment_productBuildVersion -- sets the environment.productBuildVersion (and the environmentPropList.productBuildVersion) property (does not work while debugging a Lingo Script)
	* setTheProductVersion string productVersion -- sets the productVersion (and the environment.productVersion, and the environmentPropList.productVersion) property (does not work while debugging a Lingo Script)
	* setTheEnvironment_osVersion string environment_osVersion -- sets the environment.osVersion (and the environmentPropList.osVersion) property (does not work while debugging a Lingo Script)
	* setTheMachineType int machineType -- sets the machineType property (does not work while debugging a Lingo Script)
	-- Force Properties
	* forceTheExitLock int exitLock -- forces the exitLock property
	* forceTheSafePlayer int safePlayer -- forces the safePlayer property
	-- Set External Params
	* setExternalParam string name, string value -- sets an External Param (name cannot be empty)
	-- Disable Functions
	* disableGoToNetMovie -- disables goToNetMovie (does not work during authoring)
	* disableGoToNetPage -- disables goToNetPage (does not work during authoring)
	-- Bugfixes
	* bugfixShockwave3DBadDriverList -- bugfixes Shockwave 3D Bad Driver List (does not work during authoring)