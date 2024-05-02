workspace "Ephyra"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

group "Ephyra"

project "Ephyra"
	location "ephyra"
	kind "StaticLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("build/" .. outputdir .. "/%{prj.name}")
	
	pchheader "ephyra_pch.h"
	pchsource "ephyra/precompiled/ephyra_pch.cpp"
	
	files
	{
		"%{prj.name}/enginecode/**.h",
		"%{prj.name}/enginecode/**.cpp",
		"ephyra/precompiled/ephyra_pch.h",
		"ephyra/precompiled/ephyra_pch.cpp"
	}

	includedirs
	{
		"%{prj.name}/enginecode/",
		"%{prj.name}/enginecode/include/independent",
		"%{prj.name}/enginecode/include/",
		"%{prj.name}/precompiled/",
		"vendor/assimp/include",
		"vendor/spdlog/include",
		"vendor/glfw/include",
		"vendor/Glad/include",
		"vendor/glm/",
		"vendor/STBimage",
		"vendor/freetype2/include",
		"vendor/json/single_include/nlohmann",
		"vendor/enTT"
	}
	
	links 
	{
		"Assimp",
		"GLFW",
		"Glad",
		"Freetype"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"NG_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "NG_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "NG_RELEASE"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("build/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/include/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/include",
		"ephyra/enginecode/",
		"ephyra/enginecode/include/Core",
		"ephyra/enginecode/include/",
		"ephyra/precompiled/",
		"vendor/assimp/include",
		"vendor/glfw/include",
		"vendor/Glad/include",
		"vendor/glm/",
		"vendor/spdlog/include",
		"vendor/freetype2/include",
		"vendor/json/single_include/nlohmann",
		"vendor/enTT"
	}

	links
	{
		"Ephyra"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"NG_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "NG_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "NG_RELEASE"
		runtime "Release"
		optimize "On"

group "Vendor"

	include "vendor/glfw"
	include "vendor/Glad"
	include "vendor/freetype2"
	include "vendor/Assimp"

