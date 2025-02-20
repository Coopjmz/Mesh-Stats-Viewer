@echo off

pushd ..\..
tools\Premake\Windows\premake5.exe --verbose vs2017
popd
pause