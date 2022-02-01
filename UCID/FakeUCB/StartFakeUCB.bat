SETLOCAL
set configuration=Release
set platform=x64

echo WARNING: Remember to build LibPmUtil with CM_HTTP_ENABLED preprocessor

xcopy /s "..\\FakeCMIDApiDll\\%platform%\\%configuration%\\cmidapi.dll" "C:\\Program Files\\Cisco\\Cisco Secure Client\\CM\\9.0.0.0\\CMID\\1.1.110.0\\%platform%\\cmidapi.dll" /y

"..\\..\\%platform%\\%configuration%\\FakeUCB.exe"