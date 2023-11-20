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
IncludeDir["ImGui"] = "rocket-engine/vendors/imgui"
IncludeDir["glm"] = "rocket-engine/vendors/glm"
IncludeDir["stb"] = "rocket-engine/vendors/stb_image"
IncludeDir["entt"] = "rocket-engine/vendors/entt"

startproject "rocket-editor"

include "rocket-engine/vendors/GLFW"
include "rocket-engine/vendors/Glad"
include "rocket-engine/vendors/imgui"

project "rocket-engine"
	location "rocket-engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "rcktpch.h"
	pchsource "rocket-engine/src/rcktpch.cpp"

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendors/stb_image/**.cpp",
		"%{prj.name}/vendors/stb_image/**.h"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendors/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.entt}/include"
	}

	links 
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

		defines 
		{
			 "RCKT_PLATFORM_WINDOWS",
			 "RCKT_BUILD_DLL",
			 "GLFW_INCLUDE_NONE",
			 "RCKT_ENABLE_ASSERTS"
		}

	filter "configurations:Debug"
		defines "RCKT_DEBUG"
		runtime "Debug"
		symbols "on"

	
	filter "configurations:Release"
		defines "RCKT_RELEASE"
		runtime "Release"
		optimize "on"

	
	filter "configurations:Dist"
		defines "RCKT_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	defines 
	{
		"RCKT_PLATFORM_WINDOWS"
	}

	includedirs 
	{
		"rocket-engine/vendors/spdlog/include",
		"rocket-engine/vendors/glm",
		"rocket-engine/vendors/Glad/include",
		"rocket-engine/src",
		"%{IncludeDir.entt}/include"
	}

	links
	{
		"rocket-engine"
	}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

	filter "configurations:Debug"
		defines "RCKT_DEBUG"
		runtime "Debug"
		symbols "on"

	
	filter "configurations:Release"
		defines "RCKT_RELEASE"
		runtime "Release"
		optimize "on"

	
	filter "configurations:Dist"
		defines "RCKT_DIST"
		runtime "Release"
		optimize "On"


	project "rocket-editor"
		location "rocket-editor"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++20"
		staticruntime "on"
	
		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
		files 
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp"
		}
	
		defines 
		{
			"RCKT_PLATFORM_WINDOWS",
		}
	
		includedirs 
		{
			"rocket-engine/vendors/spdlog/include",
			"rocket-engine/vendors/glm",
			"rocket-engine/vendors/Glad/include",
			"rocket-engine/src",
			"%{IncludeDir.entt}/include"
		}
	
		links
		{
			"rocket-engine"
		}
	
		filter "system:windows"
			cppdialect "C++20"
			systemversion "latest"
	
		filter "configurations:Debug"
			defines "RCKT_DEBUG"
			runtime "Debug"
			symbols "on"
	
		
		filter "configurations:Release"
			defines "RCKT_RELEASE"
			runtime "Release"
			optimize "on"
	
		
		filter "configurations:Dist"
			defines "RCKT_DIST"
			runtime "Release"
			optimize "On"