-- premake5.lua
workspace "Lullaby Engine"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "LullabyEditor"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

include "LullabyCore/Build-Core.lua"
include "LullabyEditor/Build-Editor.lua"