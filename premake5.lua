workspace "MandelbrotSet"
	architecture "x64"
	startproject "MandelbrotSet"

	configurations { "Debug", "Release" }
	flags { "MultiProcessorCompile" }

	BinaryOutputDirectory = "Binaries/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	OBJOutputDirectory = "Binaries/Intermediate/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "External"
	include "MandelbrotSet/External/GLFW"
	include "MandelbrotSet/External/Glad"
	include "MandelbrotSet/External/imgui"
group ""

project "MandelbrotSet"
	location "MandelbrotSet"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir (BinaryOutputDirectory)
	objdir (OBJOutputDirectory)

	files { 
 		"MandelbrotSet/Source/**.h",
		"MandelbrotSet/Source/**.cpp",
		"MandelbrotSet/External/stb/stb_image_write.cpp"
	}
	includedirs { 
		"MandelbrotSet/Source",
		"MandelbrotSet/External/GLFW/include",
		"MandelbrotSet/External/Glad/include",
		"MandelbrotSet/External/imgui",
		"MandelbrotSet/External/stb"
	}

	links { "GLFW", "Glad", "ImGui", "opengl32.lib" }
	defines { "_CRT_SECURE_NO_WARNINGS" }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		symbols "On"
	filter "configurations:Release"
		optimize "On"