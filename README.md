# SMG2 Project Template
You can find here the source code for all custom coded objects and changes featured in the **SMG2 Project Template**. Compiled builds and the actual download can be found under **Releases**. This requires [Syati](https://github.com/Evanbowl/Syati), a toolchain by shibbo to inject custom code into SMG2.
You can find more information and videos on [my website](https://aurumsmods.com/#project-template).

This fork of Project Template has compiler flags. You can add `--gle` to the build command to disable all patches that conflict with SuperHackio's [GalaxyLevelEngine](https://github.com/SuperHackio/GalaxyLevelEngine).

Build flag `--all` builds absolutely everything in the workspace, regardless of GLE compatibility.

Example
`py build.py USA --gle`

There are other flags, but they are reserved for the projects of others.
