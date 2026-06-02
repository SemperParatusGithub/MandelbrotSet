file(GLOB MANDELBROT_SOURCES CONFIGURE_DEPENDS
    "Source/*.cpp"
    "Source/*.h"
)

add_executable(MandelbrotSet ${MANDELBROT_SOURCES})

target_include_directories(MandelbrotSet PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

target_link_libraries(MandelbrotSet PRIVATE
    glfw
    Glad
    ImGui
    stb
    OpenGL::GL
)

target_compile_definitions(MandelbrotSet PRIVATE
    _CRT_SECURE_NO_WARNINGS
    # MSVC defines _DEBUG automatically with the debug runtime; mirror that on
    # other compilers so the LOG_INFO / LOG_ERROR macros in Core.h light up.
    $<$<CONFIG:Debug>:_DEBUG>
)

# When the user hits F5 in Visual Studio (or runs from Xcode) we want the
# working directory to be MandelbrotSet/, so that the relative paths
# `Shaders/...glsl` and `Fonts/OpenSans.ttf` resolve.
set_target_properties(MandelbrotSet PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    XCODE_GENERATE_SCHEME ON
    XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# For command-line / Ninja / Make builds the binary is dropped under Binaries/.
# Copy the runtime assets next to it so the executable can be launched
# directly from there as well.
add_custom_command(TARGET MandelbrotSet POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
        "$<TARGET_FILE_DIR:MandelbrotSet>/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Fonts"
        "$<TARGET_FILE_DIR:MandelbrotSet>/Fonts"
    VERBATIM
)
