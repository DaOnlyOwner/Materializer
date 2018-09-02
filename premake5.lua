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
        filter{"system:linux"}
            links "glfw" -- cannot link against glfw3 on Linux, has to be glfw
        filter{"system:windows"}
            links "glfw3"
        filter{}
    end

    function opengl_link()
        filter {"system:windows"}
            links {"OpenGL32"}
        filter {"system:not windows"}
            links {"GL"}
            links {"dl"} -- required by glad on non-windows systems
        filter{}
    end

    function halide_link()
        filter {"configurations:Debug", "system:windows"}
            links "Halided"
        filter {"configurations:Release", "system:windows"}
            links "Halide"
        filter {"system:linux"}
            links "Halide" -- always link against Halide, (has no specific meaning, was too lazy to build it as debug target)
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

        

    
