@echo off
pushd ..\
call "vendors\premake\bin\premake5.exe" vs2022
popd
PAUSE
