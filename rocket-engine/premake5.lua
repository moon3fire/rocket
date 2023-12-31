project "rocket-engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "rcktpch.h"
	pchsource "src/rcktpch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendors/stb_image/**.h",
		"vendors/stb_image/**.cpp",
		"vendors/glm/glm/**.hpp",
		"vendors/glm/glm/**.inl",

		"vendors/ImGuizmo/ImGuizmo.h",
		"vendors/ImGuizmo/ImGuizmo.cpp"
	}

	defines
	{
		"YAML_CPP_STATIC_DEFINE",
		"RCKT_PLATFORM_WINDOWS",
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
	}

	includedirs
	{
		"src",
		"vendors/spdlog/include",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"Box2D",
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib"
	}

	filter "files:vendors/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
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