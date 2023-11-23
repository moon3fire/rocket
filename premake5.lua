include "./vendors/premake/premake_customization/solution_items.lua"

workspace "rocket"
	architecture "x86_64"
	startproject "rocket-editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/rocket-engine/vendors/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/rocket-engine/vendors/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/rocket-engine/vendors/imgui"
IncludeDir["glm"] = "%{wks.location}/rocket-engine/vendors/glm"
IncludeDir["stb_image"] = "%{wks.location}/rocket-engine/vendors/stb_image"
IncludeDir["entt"] = "%{wks.location}/rocket-engine/vendors/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/rocket-engine/vendors/yaml-cpp/include"

group "Dependencies"
	include "vendors/premake"
	include "rocket-engine/vendors/GLFW"
	include "rocket-engine/vendors/Glad"
	include "rocket-engine/vendors/imgui"
	include "rocket-engine/vendors/yaml-cpp"
group ""

include "rocket-engine"
include "Sandbox"
include "rocket-editor"
