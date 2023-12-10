@echo off
pushd ..\
dir
call "vendors\premake\bin\premake5.exe" vs2022
popd
PAUSE
