project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	defines
	{
		"RCKT_PLATFORM_WINDOWS"
	}

	includedirs
	{
		"%{wks.location}/rocket-engine/vendors/spdlog/include",
		"%{wks.location}/rocket-engine/src",
		"%{wks.location}/rocket-engine/vendors",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.Glad}"
	}

	links
	{
		"rocket-engine"
	}

	filter "system:windows"
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
		optimize "on"