cmake_minimum_required(VERSION 3.16)

# GLFW 3.4 ships with `cmake_minimum_required(VERSION 3.0)`, which CMake 4
# rejects unless we relax the policy version floor for nested projects.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(MandelbrotSet
    LANGUAGES C CXX
    DESCRIPTION "Real-time GPU Mandelbrot/Julia-set renderer"
)

# --- Configurations ---------------------------------------------------------
# Match premake's `configurations { "Debug", "Release" }` for single-config
# generators. Multi-config generators (Visual Studio, Xcode, Ninja Multi-Config)
# expose Debug/Release through CMAKE_CONFIGURATION_TYPES instead.
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    endif()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
else()
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif()

# --- Language standards -----------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Architecture (premake's `architecture "x64"`) --------------------------
if(CMAKE_SIZEOF_VOID_P LESS 8)
    message(FATAL_ERROR "MandelbrotSet requires a 64-bit toolchain.")
endif()

# --- MSVC-specific flags ----------------------------------------------------
# Mirrors premake's `staticruntime "On"` and `flags { "MultiProcessorCompile" }`.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/MP)
endif()

# --- Output directories -----------------------------------------------------
# Matches premake's `Binaries/<config>-<system>-<arch>/` layout. We deliberately
# avoid `$<CONFIG>` here: CMake 4.2 (shipped with CLion) doesn't reliably create
# directories whose path contains a generator expression for static-library
# targets, so we set explicit per-config variables instead.
set(_BIN_SUFFIX "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

if(CMAKE_CONFIGURATION_TYPES)
    foreach(_cfg ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${_cfg}" _cfg_upper)
        set(_dir "${CMAKE_SOURCE_DIR}/Binaries/${_cfg}-${_BIN_SUFFIX}")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_cfg_upper} "${_dir}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_cfg_upper} "${_dir}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_cfg_upper} "${_dir}")
    endforeach()
else()
    set(_dir "${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_BUILD_TYPE}-${_BIN_SUFFIX}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${_dir}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${_dir}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${_dir}")
endif()

# --- Visual Studio: pick the executable as the F5 / startup project ---------
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT MandelbrotSet)

# --- OpenGL -----------------------------------------------------------------
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# --- External dependencies --------------------------------------------------
# Disable GLFW's docs / tests / examples / install before pulling it in.
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

add_subdirectory(MandelbrotSet/External/GLFW)
add_subdirectory(MandelbrotSet/External/Glad)
add_subdirectory(MandelbrotSet/External/imgui)
add_subdirectory(MandelbrotSet/External/stb)

# --- Application ------------------------------------------------------------
add_subdirectory(MandelbrotSet)
