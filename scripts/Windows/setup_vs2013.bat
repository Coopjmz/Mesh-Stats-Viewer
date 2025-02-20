@echo off

pushd ..\..
tools\Premake\Windows\premake5.exe --verbose vs2013
popd
pause