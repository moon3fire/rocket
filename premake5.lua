workspace "rocket-engine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "rocket-engine"
	location "rocket-engine"
	kind "SharedLib"
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
		"%{prj.name}/vendors/spdlog/include",
		"%{prj.name}/src"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines 
		{
			 "RCKT_PLATFORM_WINDOWS",
			 "RCKT_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "RCKT_DEBUG"
		symbols "On"

	
	filter "configurations:Release"
		defines "RCKT_RELEASE"
		optimize "On"

	
	filter "configurations:Dist"
		defines "RCKT_DIST"
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
		symbols "On"

	
	filter "configurations:Release"
		defines "RCKT_RELEASE"
		optimize "On"

	
	filter "configurations:Dist"
		defines "RCKT_DIST"
		optimize "On"