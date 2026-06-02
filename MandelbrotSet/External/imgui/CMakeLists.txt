# ImGui core. The OpenGL3 / GLFW backends in `examples/` are intentionally
# omitted because Source/ImGuiUtil.cpp `#include`s them directly.
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
)

target_include_directories(ImGui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_features(ImGui PUBLIC cxx_std_17)
