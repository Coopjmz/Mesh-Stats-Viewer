workspace "Mesh Stats Viewer"
    architecture "x64"
    startproject "Mesh Stats Viewer"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

BuildDir = "%{wks.location}/build"
OutDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
BinDir = "%{BuildDir}/bin/%{OutDir}"
ObjDir = "%{BuildDir}/obj/%{OutDir}"

ExternalDir = "%{wks.location}/external"

IncludeDirs = {}
IncludeDirs["GLFW"] = "%{ExternalDir}/glfw/include"

LibDirs = {}
LibDirs["GLFW"] = "%{ExternalDir}/glfw/lib"

include "Mesh Stats Viewer"