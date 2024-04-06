# SMG2 Project Template
You can find here the source code for all custom coded objects and changes featured in the **SMG2 Project Template**. This requires [Syati](https://github.com/SMGCommunity/Syati), a toolchain by shibbo to inject custom code into SMG2.
To set it up properly, you'll need the CodeWarrior compiler and Kamek linker, which you can quickly set up by using [SyatiSetup](https://github.com/Lord-Giganticus/SyatiSetup).

This fork of Project Template has compiler flags. You can add `--gle` to the build command to disable all patches that conflict with SuperHackio's [GalaxyLevelEngine](https://github.com/SuperHackio/GalaxyLevelEngine).

Build flag `--all` builds absolutely everything in the workspace, regardless of GLE compatibility.

Example:
`py build.py USA --gle`

Compiled builds are supplied but use of them is not recommended. The custom assets required for some patches are no longer maintaned on the repository.

There are other build flags, but they are reserved for the projects of others.

-ALL (Builds everything- this is extremely unstable!)<br/>
-NOGLE<br/>
-GLE<br/>
-SMSS (SPG64's project "Super Mario Starshine")<br/>
-CA (Mariokirby1703's project "SMG2 Collector's Anxiety")<br/>
-SMG63 (Alex SMG's project "Super Mario Galaxy 63")<br/>
-USEBLUECOIN (Build the BlueCoinSystem)
-SM64BLUECOIN (Build the BlueCoins to function like in SM64)
-GSTANDPAD (Build Aurum's GST and PAD recorders)
-KBD (Build the experimental keyboard library)

More flags can be added, however this project is already hard enough to maintain.<br/>
It's worth it though!
