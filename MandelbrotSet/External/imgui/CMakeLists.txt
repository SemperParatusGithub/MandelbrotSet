# ImGui core + the OpenGL3 / GLFW backends so consumers don't have to
# `#include` the .cpp files directly (which forced a 40 kLOC recompile every
# time anything in ImGuiUtil.cpp changed).
add_library(ImGui STATIC
    imconfig.h
    imgui.h
    imgui.cpp
    imgui_draw.cpp
    imgui_internal.h
    imgui_widgets.cpp
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    imgui_demo.cpp

    examples/imgui_impl_glfw.h
    examples/imgui_impl_glfw.cpp
    examples/imgui_impl_opengl3.h
    examples/imgui_impl_opengl3.cpp
)

target_include_directories(ImGui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples"
)

# imgui_impl_opengl3.cpp picks its loader at compile time; we use Glad.
target_compile_definitions(ImGui PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)

target_link_libraries(ImGui PUBLIC glfw Glad)

target_compile_features(ImGui PUBLIC cxx_std_17)
