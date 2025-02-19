project "Mesh Stats Viewer"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir (BinDir)
    objdir (ObjDir)

    pchheader "pch.h"
    pchsource "src/pch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp",
        "vendor/**.h",
        "vendor/**.cpp"
    }

    includedirs
    {
        "src",
        "vendor",
        "%{IncludeDirs.GLFW}"
    }

    libdirs
    {
        "%{LibDirs.GLFW}"
    }

    links
    {
        "glfw3_mt",
        "opengl32"
    }

    defines
    {
        "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
    }

    filter "files:vendor/**.cpp"
        flags "NoPCH"

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        defines "DEBUG"
        kind "ConsoleApp"
        optimize "Off"
        symbols "On"
        linkoptions "/NODEFAULTLIB:LIBCMT"

    filter "configurations:Release"
        runtime "Release"
        defines "RELEASE"
        kind "WindowedApp"
        optimize "On"
        symbols "Off"