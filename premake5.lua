workspace "MaterializerWorkspace"
    language "C++"
    cppdialect "C++17"
    architecture "x86_64"
    configurations {"Debug", "Release"}
    systemversion "10.0.16299.0"
    targetdir ("build/bin/%{prj.name}/%{cfg.longname}")
    objdir ("build/obj/%{prj.name}/%{cfg.longname}")
    location "build"
    filter {"configurations:Debug"}
        defines {"DEBUG"}
        --symbols "On"
        optimize "Debug"
    filter {"configurations:Release"}
        defines {"NDEBUG"}
        optimize "Full"
        symbols "Off"
    filter {}

    filter {"system:windows"}
        libdirs("E:/Libraries/lib")
        includedirs("E:/Libraries/include")
        bindirs("E:/Libraries/bin")
    filter {"system:linux"}
        libdirs ("/usr/lib")
        includedirs ("/usr/include")
        bindirs("/usr/bin")

    filter {}


    function glfw_link()
        links "glfw3"
    end

    function opengl_link()
        filter {"system:windows"}
            links {"OpenGL32"}
        filter {"system:not windows"}
            links {"GL"}
        filter{}
    end

    function halide_link()
        filter {"configurations:Debug"}
            links "Halided"
        filter {"configurations:Release"}
            links "Halide"
        filter{}
    end

project "Materializer"
    kind "ConsoleApp"
    files {"Main/src/**", "Main/lib/glad33/**", "Main/lib/imgui/*"}
    includedirs {"Main/lib/glad33/include","Main/lib/imgui"}
    halide_link()
    glfw_link()
    opengl_link()

--project "NodeLibrary"
--    kind "ConsoleApp"
--    files {"HalideTools/GenGen.cpp", "NodeLibrary/src/**"}
--    halide_link()
--    postbuildcommands {
--       "%{wks.location}%{cfg.targetdir}\\NodeLibrary -g BlendGenerator -o . target=host"
--    }

        

    