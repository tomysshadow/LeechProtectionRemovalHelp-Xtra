LeechProtectionRemovalHelp Xtra 1.4.1
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
	-10.1.1.16
	-11.0.0.426
	-11.0.3.466 (11 Hotfix 3)
	-11.5.0.593
	-11.5.8.612
	-11.5.9.629
	-11.6.5.635
	-12.0.0.111

	Compatibility with untested versions is not guaranteed.


Usage
	on exitFrame me
		setTheMoviePath("http://addictinggames.com/newGames/metalmayhemworldtour/")
		setTheMovieName("metalmayhemworldtour.dcr")
		setTheEnvironment_shockMachine(0)
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
	new object me
	-- Set Properties --
	* setTheMoviePath string moviePath -- sets the moviePath property
	-- and the path, and the pathName, and _movie.path
	* setTheMovieName string movieName -- sets the movieName property
	-- and the movie, and _movie.name
	* setTheEnvironment_shockMachine integer environment_shockMachine -- sets the environment.shockMachine property
	-- and the environmentPropList.shockMachine, and _system.environmentPropList.shockMachine
	* setTheEnvironment_shockMachineVersion string environment_shockMachineVersion -- sets the environment.shockMachineVersion property
	-- and the environmentPropList.shockMachineVersion, and _system.environmentPropList.shockMachineVersion
	* setThePlatform string platform -- sets the platform property
	-- and the environment.platform, and the environmentPropList.platform, and _system.environmentPropList.platform
	* setTheRunMode string runMode -- sets the runMode property
	-- and the environment.runMode, and the environmentPropList.runMode, and _system.environmentPropList.runMode, and _player.runMode
	* setTheEnvironment_productBuildVersion string environment_productBuildVersion -- sets the environment.productBuildVersion property
	-- and the environmentPropList.productBuildVersion, and _system.environmentPropList.productBuildVersion
	* setTheProductVersion string productVersion -- sets the productVersion property
	-- and the environment.productVersion, and the environmentPropList.productVersion, and _system.environmentPropList.productVersion, and _player.productVersion
	* setTheEnvironment_osVersion string environment_osVersion -- sets the environment.osVersion property
	-- and the environmentPropList.osVersion, and _system.environmentPropList.osVersion
	* setTheMachineType integer machineType -- sets the machineType property
	-- Set External Params --
	* setExternalParam string name, string value -- sets an External Param
	-- name cannot be empty
	-- Force Properties --
	* forceTheExitLock integer exitLock -- forces the exitLock property
	* forceTheSafePlayer integer safePlayer -- forces the safePlayer property
	-- Disable Functions --
	* disableGoToNetMovie -- disables goToNetMovie
	* disableGoToNetPage -- disables goToNetPage
	-- Bugfixes --
	* bugfixShockwave3DBadDriverList -- bugfixes Shockwave 3D Bad Driver List