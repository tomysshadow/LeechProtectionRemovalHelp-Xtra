LeechProtectionRemovalHelp Xtra 1.2.0
By Anthony Kleine

	This Director Xtra allows for the archival of Shockwave Movies
	by setting, forcing, disabling or bugfixing properties
	which will stay set, forced, disabled or bugfixed into
	the next movie in the playlist.

	The LeechProtectionRemovalHelp Xtra is only compatible
	with Director 8.5.0.321, 10.0.0.188, and 11.5.0.593 as well as
	their associated Xtras.


Usage:
	on exitFrame
		setTheMoviePath("http://addictinggames.com/newGames/metalmayhemworldtour/")
		setTheRunMode("Author")
		setThePlatform("Windows,32")
		setTheMachineType(256)
		setTheEnvironment_ShockMachine(1)
		forceTheExitLock(0)
		setExternalParam("src", "/newGames/metalmayhemworldtour/metalmayhemworldtour.dcr")
		setExternalParam("sw2", "121220")
		disableGoToNetMovie()
		disableGoToNetPage()
		bugfixShockwave3DBadDriverList()
		go(1, "metalmayhemworldtour.dcr")
	end

	-- Set Property Handlers -- 
	* setTheMoviePath string moviePath -- sets the moviePath property
	* setTheRunMode string runMode -- sets the runMode property
	* setThePlatform string platform -- sets the platform property
	* setTheMachineType int machineType -- sets the machineType property
	* setTheEnvironment_ShockMachine int environment_ShockMachine -- sets the environment.shockMachine property
 	-- Force Property Handlers -- 
	* forceTheExitLock int exitLock -- forces the exitLock property
	-- Set External Param Handlers --
	* setExternalParam string name, string value -- sets an External Param
 	-- Disable Handler Handlers -- 
	* disableGoToNetMovie -- disables goToNetMovie
	* disableGoToNetPage -- disables goToNetPage
 	-- Bugfix Handlers -- 
	* bugfixShockwave3DBadDriverList -- bugfixes Shockwave 3D Bad Driver List