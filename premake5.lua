workspace "Brigerad"
    architecture "x64"
    startproject "BrigeradEditor"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution folder)
IncludeDir = {}
IncludeDir["GLFW"] = "Brigerad/vendor/GLFW/include"
IncludeDir["Glad"] = "Brigerad/vendor/Glad/include"
IncludeDir["ImGui"] = "Brigerad/vendor/ImGui"
IncludeDir["glm"] = "Brigerad/vendor/glm"
IncludeDir["stb_image"] = "Brigerad/vendor/stb_image"
IncludeDir["serial"] = "Brigerad/vendor/serial"
IncludeDir["entt"] = "Brigerad/vendor/entt/include"
IncludeDir["lua"] = "Brigerad/vendor/lua"
IncludeDir["sol"] = "Brigerad/vendor/sol"
IncludeDir["yaml_cpp"] = "Brigerad/vendor/yaml-cpp/include"

group "Dependencies"
    include "Brigerad/vendor/GLFW"
    include "Brigerad/vendor/Glad"
    include "Brigerad/vendor/ImGui"
    include "Brigerad/vendor/lua"
    include "Brigerad/vendor/yaml-cpp"

group ""

project "Brigerad"
    location "Brigerad"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "brpch.h"
    pchsource "Brigerad/src/brpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/glm/**.hpp",
        "%{prj.name}/vendor/glm/**.inl",
        "%{prj.name}/vendor/serial/**.h",
        "%{prj.name}/vendor/serial/**.cc",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.serial}/include",
        "%{IncludeDir.entt}",
        "%{IncludeDir.lua}/src",
        "%{IncludeDir.sol}/include",
        "%{IncludeDir.yaml_cpp}"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "BR_PLATFORM_WINDOWS",
            "BR_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        links
        {
            "GLFW",
            "Glad",
            "ImGui",
            "lua",
            "opengl32.lib",
            "yaml-cpp"
        }

        excludes
        {
            "%{prj.name}/src/Platform/Linux/**.h",
            "%{prj.name}/src/Platform/Linux/**.cpp",
        }

    filter "system:linux"
        systemversion "latest"

        defines
        {
            "BR_PLATFORM_LINUX",
            "GLFW_INCLUDE_NONE"
        }

        links
        {
            "GL",
            "m",
            "dl",
            "Xinerama",
            "Xrandr",
            "Xi",
            "Xcursor",
            "X11",
            "Xxf86vm",
            "pthread",
            "GLFW",
            "Glad",
            "lua",
            "ImGui"
        }

        excludes
        {
            "%{prj.name}/src/Platform/Windows/**.h",
            "%{prj.name}/src/Platform/Windows/**.cpp"
        }

    filter "configurations:Debug"
        defines 
        {
            "BR_DEBUG",
            "BR_ENABLE_ASSERTS",
            "BR_PROFILE"
        }
        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        defines "BR_RELEASE"
        runtime "Release"
        optimize "on"
        
    filter "configurations:Dist"
        defines "BR_DIST"
        runtime "Release"
        optimize "on"


project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"

    language "C++"
    cppdialect "C++17"
    staticruntime "On"

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
        "%{IncludeDir.serial}/include",
        "%{IncludeDir.entt}"
    }

    filter "system:windows"
        systemversion "latest"
    
        defines
        {
            "BR_PLATFORM_WINDOWS",
        }

        links
        {
            "Brigerad",
            "GLFW",
            "Glad",
            "ImGui",
            "lua",
            "opengl32.lib"
        }
    
    filter "system:linux"
        systemversion "latest"

        defines
        {
            "BR_PLATFORM_LINUX"
        }

        links
        {
            "Brigerad",
            "GL",
            "m",
            "dl",
            "Xinerama",
            "Xrandr",
            "Xi",
            "Xcursor",
            "X11",
            "Xxf86vm",
            "pthread",
            "GLFW",
            "Glad",
            "lua",
            "ImGui",
        }

        postbuildcommands{"cp -r assets ../bin/" .. outputdir .. "/%{prj.name}"}
            
        filter "configurations:Debug"
        defines 
        {
            "BR_DEBUG",
            "BR_PROFILE"
        }
        runtime "Debug"
        symbols "On"
            
    filter "configurations:Release"
        defines "BR_RELEASE"
        runtime "Release"
        optimize "On"
            
    filter "configurations:Dist"
        defines "BR_DIST"
        runtime "Release"
        optimize "On"
    

project "BrigeradEditor"
    location "BrigeradEditor"
    kind "ConsoleApp"

    language "C++"
    cppdialect "C++17"
    staticruntime "On"

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
        "%{IncludeDir.serial}/include",
        "%{IncludeDir.entt}",
        "%{IncludeDir.ImGui}"
    }

    filter "system:windows"
        systemversion "latest"
    
        defines
        {
            "BR_PLATFORM_WINDOWS",
        }

        links
        {
            "Brigerad",
            "GLFW",
            "Glad",
            "ImGui",
            "lua",
            "opengl32.lib"
        }
    
        postbuildcommands("xcopy assets ..\\bin\\" .. outputdir .. "\\%{prj.name}\\assets /K /I /Y /S")

    filter "system:linux"
        systemversion "latest"

        defines
        {
            "BR_PLATFORM_LINUX",
        }

        links
        {
            "Brigerad",
            "GL",
            "m",
            "dl",
            "Xinerama",
            "Xrandr",
            "Xi",
            "Xcursor",
            "X11",
            "Xxf86vm",
            "pthread",
            "GLFW",
            "Glad",
            "lua",
            "ImGui",
        }

        postbuildcommands{"cp -r assets ../bin/" .. outputdir .. "/%{prj.name}"}
            
        filter "configurations:Debug"
        defines 
        {
            "BR_DEBUG",
            "BR_PROFILE"
        }
        runtime "Debug"
        symbols "On"
            
    filter "configurations:Release"
        defines "BR_RELEASE"
        runtime "Release"
        optimize "On"
            
    filter "configurations:Dist"
        defines "BR_DIST"
        runtime "Release"
        optimize "On"
    

