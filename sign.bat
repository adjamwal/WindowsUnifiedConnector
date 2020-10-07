@echo OFF
rem Batch files and build events don't play nice. This doesn't work from the post build event
rem Calling set USE_SIGNING_TIMESTAMP=1 && "%IMN_COMMON%\common-windows-build\Tools\signing\signingvs10_noph.bat" %1 %2

set USE_SIGNING_TIMESTAMP=1
set USE_SIGNTOOL_AC=0
"%IMN_COMMON%\common-windows-build\Tools\signing\signingvs10_noph.bat" %1 %2