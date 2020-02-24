workspace "Brigerad"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Brigerad"
    location "Brigerad"
    kind "SharedLib"
    language "C++"

    targetdire ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "brpch.h"
    pchsource "Brigerad/src/brpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "BR_PLATFORM_WINDOWS",
            "BR_BUILD_DLL",
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }

    filter "configurations:Debug"
        defines "BR_DEBUG"
        symbols "On"
        
    filter "configurations:Release"
        defines "BR_RELEASE"
        optimize "On"
        
    filter "configurations:Dist"
        defines "BR_DIST"
        optimize "On"


project "Sandbox"
        location "Sandbox"
        kind "ConsoleApp"

        language "C++"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp",
        }
    
        includedirs
        {
            "Brigerad/vendor/spdlog/include",
            "Brigerad/src"
        }
    
        links
        {
            "Brigerad"
        }

        filter "system:windows"
            cppdialect "C++17"
            staticruntime "On"
            systemversion "latest"
    
            defines
            {
                "BR_PLATFORM_WINDOWS",
            }
    
        filter "configurations:Debug"
            defines "BR_DEBUG"
            symbols "On"
            
        filter "configurations:Release"
            defines "BR_RELEASE"
            optimize "On"
            
        filter "configurations:Dist"
            defines "BR_DIST"
            optimize "On"
    

    