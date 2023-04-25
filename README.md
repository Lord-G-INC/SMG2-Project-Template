# SMG2 Project Template
You can find here the source code for all custom coded objects and changes featured in the **SMG2 Project Template**. This requires [Syati](https://github.com/Evanbowl/Syati), a toolchain by shibbo to inject custom code into SMG2.
You can find more information and videos on [my website](https://aurumsmods.com/#project-template).

This fork of Project Template has compiler flags. You can add `--gle` to the build command to disable all patches that conflict with SuperHackio's [GalaxyLevelEngine](https://github.com/SuperHackio/GalaxyLevelEngine).

Build flag `--all` builds absolutely everything in the workspace, regardless of GLE compatibility.

Example:
`py build.py USA --gle`

Compiled builds are supplied but use of them is not recommended. The custom assets required for some patches are no longer maintaned on the repository.

There are other build flags, but they are reserved for the projects of others.

-ALL (Builds everything- this is extremely unstable!)<br/>
-GLE<br/>
-NOGLE<br/>
-SMSS (SPG64's project "Super Mario Starshine")<br/>
-CA (Mariokirby1703's project "SMG2 Collector's Anxiety")<br/>
-SMG63 (Alex SMG's project "Super Mario Galaxy 63")<br/>

More flags can be added, however this project is already hard enough to maintain.<br/>
It's worth it though!
