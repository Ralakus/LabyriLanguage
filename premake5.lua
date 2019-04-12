
is_64_bit = true

workspace "Labyri"
    language "C"
    cdialect "C11"

    location "build"

    if is_64_bit then
        architecture "x86_64"
    else
        architecture "x86"
    end

    configurations { "Debug", "Release" }

    filter { "configurations:Debug" }
            symbols "On"

    filter { "configurations:Release" }
            optimize "On"
    filter { }

    targetdir ("build/bin/")

    objdir ("build/obj/")


project "lab"
    kind "ConsoleApp"

    includedirs "src"
    includedirs "lib/LabyriCUtils/src"

    files { "src/**.h", "src/**.c"}

    if is_64_bit then
        defines { "LAB_64_BIT" }
    else 
        defines { "LAB_32_BIT" }
    end

    filter "configurations:Debug"
        defines { "DEBUG" }

    filter "configurations:Release"
        defines { "NDEBUG" }

    filter {}

    libdirs { "lib/LabyriCUtils/build/bin/" }
    links { "labcutils" }