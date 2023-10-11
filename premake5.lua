workspace "rocket-engine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "rocket-engine/vendors/GLFW/include"
IncludeDir["Glad"] = "rocket-engine/vendors/Glad/include"

include "rocket-engine/vendors/GLFW"
include "rocket-engine/vendors/Glad"

project "rocket-engine"
	location "rocket-engine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "rcktpch.h"
	pchsource "rocket-engine/src/rcktpch.cpp"

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendors/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}"
	}

	links 
	{
		"GLFW",
		"Glad",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines 
		{
			 "RCKT_PLATFORM_WINDOWS",
			 "RCKT_BUILD_DLL",
			 "GLFW_INCLUDE_NONE",
			 "RCKT_ENABLE_ASSERTS"
		}

		postbuildcommands
		{
    		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}


	filter "configurations:Debug"
		defines "RCKT_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	
	filter "configurations:Release"
		defines "RCKT_RELEASE"
		buildoptions "/MD"
		optimize "On"

	
	filter "configurations:Dist"
		defines "RCKT_DIST"
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
		"%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
		"rocket-engine/vendors/spdlog/include",
		"rocket-engine/src"
	}

	links
	{
		"rocket-engine"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines 
		{
			 "RCKT_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "RCKT_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	
	filter "configurations:Release"
		defines "RCKT_RELEASE"
		buildoptions "/MD"
		optimize "On"

	
	filter "configurations:Dist"
		defines "RCKT_DIST"
		buildoptions "/MD"
		optimize "On"