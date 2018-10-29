Is64bit  = true
UseClang = true

workspace "Labyrinth"
    language "C"
    cdialect "C99"

    configurations { "Debug", "Release" }

    filter { "configurations:Debug" }
        symbols "On"
        optimize "Off"

    filter { "configurations:Release" }
        optimize "On"
        symbols "Off"
    filter { }

    filter { "system:windows"}
        defines { "LAB_WINDOWS" }
    filter { "system:linux" }
        defines { "LAB_LINUX" }
    filter { }

    if Is64bit then
        location "build64"

        architecture "x86_64"
        defines { "LAB_64" }

        targetdir ("build64/bin/%{prj.name}/%{cfg.longname}")

        objdir ("build64/obj/%{prj.name}/%{cfg.longname}")
    else 
        location "build32"

        architecture "x86"
        defines { "LAB_32" }

        targetdir ("build32/bin/%{prj.name}/%{cfg.longname}")

        objdir ("build32/obj/%{prj.name}/%{cfg.longname}")
    end

    if UseClang then
        toolset "clang"
    end

project "Language"

    kind "ConsoleApp"

    files { "src/**.h", "src/**.c"  }

    includedirs { "src/" }

    links { "m" }

    filter "configurations:Debug"
            defines { "DEBUG" }

    filter "configurations:Release"
            defines { "NDEBUG" }
    filter { }
