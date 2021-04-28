# UCID Project

## About
We need to sign the ucid binaries ourselves. Two projects will copy the files from the resources folder and sign them individualy.

The following must be signed:
- ucid.exe
- ucidapi.dll
- ucidcontrolplugin.dll

Once they are signed they are placed in the respective TargetDir of the solution. 
Both x86 and x64 must be built so ther projects are split out to logically build both for each platform/configuration selection

## Adding a new UCID build
Simply copy the ucid files to the SolutionDirectory/Resources/UCID folder
If the internal folder structure of the x64 and x86 ucid folders change, you must ensure that is propagated to the UCID_x64 and UCID_x86 and Installer projects