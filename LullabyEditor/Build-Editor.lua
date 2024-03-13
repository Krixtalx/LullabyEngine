project "LullabyEditor"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "On"

   files { "Source/**.h", "Source/**.cpp", "Source/**.rc" }

   includedirs
   {
      "Source",
	  -- Include Core
	  "../LullabyCore/Source",
      "%{VULKAN_SDK}/Include",
      "%{wks.location}/Dependencies/VMA/include",
      "%{wks.location}/Dependencies/vkBootstrap",
      "%{wks.location}/Dependencies/glfw/include",
      "%{wks.location}/Dependencies/glm",
      "%{wks.location}/Dependencies/tinyobjloader"
   }

   links
   {
      "LullabyCore"
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       kind("WindowedApp")
       defines { "DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"