project "LullabyCore"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "Off"

   pchheader "LullabyPch.h"
   pchsource ("Source/LullabyPch.cpp")

   files { "Source/**.h", "Source/**.cpp", "Source/Lullaby/**.h", "Source/Lullaby/**/**.h", "Source/Lullaby/**/**.cpp", "../Dependencies/vkBootstrap/**.cpp"}

   includedirs
   {
        "Source",
		"%{VULKAN_SDK}/Include",
        "%{wks.location}/Dependencies/VMA/include",
        "%{wks.location}/Dependencies/vkBootstrap",
	    "%{wks.location}/Dependencies/glfw/include",
        "%{wks.location}/Dependencies/glm",
        "%{wks.location}/Dependencies/tinyobjloader"
   }

   libdirs{
        "%{VULKAN_SDK}/Lib",
        "%{wks.location}/Dependencies/glfw/lib-vc2022"
   }

   links{
        "vulkan-1",
        "glfw3"
   }

   defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "files:../Dependencies/**/**.cpp"
       flags{"NoPCH"}

   filter "system:windows"
       systemversion "latest"
       defines { }

   filter "configurations:Debug"
       defines { "LullabyDebug" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "LullabyRelease" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "LullabyDist" }
       runtime "Release"
       optimize "On"
       symbols "Off"