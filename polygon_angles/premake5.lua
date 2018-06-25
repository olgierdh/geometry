workspace "geometry"
    configurations { "Debug", "Release" }

project "PolygonAngles"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    toolset "clang"

    targetdir "bin/%{cfg.buildcfg}"

    files { "**.hpp", "**.cpp" }
    
    configuration { "linux", "gmake" }
        buildoptions { "-Wall", "-Wextra", "-pedantic-errors" } 

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
