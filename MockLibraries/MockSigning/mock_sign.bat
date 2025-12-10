@echo off
REM Mock signing script for POC builds
REM Simply returns success without actual signing

echo [MOCK SIGN] Skipping code signing for: %1
echo [MOCK SIGN] POC build - no signature applied
echo [MOCK SIGN] File: %1
exit /b 0
