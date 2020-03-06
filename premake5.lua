workspace "Brigerad"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution folder)
IncludeDir = {}
IncludeDir["GLFW"] = "Brigerad/vendor/GLFW/include"
IncludeDir["Glad"] = "Brigerad/vendor/Glad/include"
IncludeDir["ImGui"] = "Brigerad/vendor/ImGui"
IncludeDir["glm"] = "Brigerad/vendor/glm"

include "Brigerad/vendor/GLFW"
include "Brigerad/vendor/Glad"
include "Brigerad/vendor/ImGui"

project "Brigerad"
    location "Brigerad"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
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
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "BR_PLATFORM_WINDOWS",
            "BR_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }

    filter "configurations:Debug"
        defines 
        {
            "BR_DEBUG",
            "BR_ENABLE_ASSERTS"
        }
        buildoptions "/MDd"
        symbols "On"
        
    filter "configurations:Release"
        defines "BR_RELEASE"
        buildoptions "/MD"
        optimize "On"
        
    filter "configurations:Dist"
        defines "BR_DIST"
        buildoptions "/MD"
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
            "Brigerad/vendor",
            "Brigerad/src",
            "%{IncludeDir.glm}",
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
            buildoptions "/MDd"
            symbols "On"
            
        filter "configurations:Release"
            defines "BR_RELEASE"
            buildoptions "/MD"
            optimize "On"
            
        filter "configurations:Dist"
            defines "BR_DIST"
            buildoptions "/MD"
            optimize "On"
    

    