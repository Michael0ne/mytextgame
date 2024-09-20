# CMakeList.txt : CMake project for MyTextGame, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("MyTextGame")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Add source to this project's executable.
add_executable (MyTextGame "src/MyTextGame.cpp" "src/MyTextGame.h")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

set(JSONCPP_WITH_TESTS OFF)
set(JSONCPP_WITH_EXAMPLE OFF)
set(SDL_SHARED OFF)
set(SDL_TEST OFF)
set(SDL_STATIC ON)
set(BUILD_SHARED_LIBS ON)

set_target_properties(MyTextGame
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY lib
    LIBRARY_OUTPUT_DIRECTORY lib
    RUNTIME_OUTPUT_DIRECTORY bin
)

target_include_directories(MyTextGame PRIVATE "src/")
target_include_directories(MyTextGame PRIVATE "src/assets/")
target_include_directories(MyTextGame PRIVATE "src/scripting/")
target_include_directories(MyTextGame PRIVATE "src/entities/")
target_include_directories(MyTextGame PRIVATE "src/audio/")
target_include_directories(MyTextGame PRIVATE "src/render/")
target_include_directories(MyTextGame PRIVATE "src/system/")
target_include_directories(MyTextGame PRIVATE "src/input/")
target_include_directories(MyTextGame PRIVATE "src/debug/")

target_include_directories(MyTextGame PRIVATE "thirdparty/xxhashct")
target_include_directories(MyTextGame PRIVATE "thirdparty/SDL/include/")
target_include_directories(MyTextGame PRIVATE "thirdparty/jsoncpp/include/json/")
target_include_directories(MyTextGame PRIVATE "thirdparty/imgui/")
target_include_directories(MyTextGame PRIVATE "thirdparty/fmt/include")
target_include_directories(MyTextGame PRIVATE "thirdparty/glm/glm")

add_subdirectory(thirdparty/SDL EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/jsoncpp)
add_subdirectory(thirdparty/fmt)
add_subdirectory(thirdparty/glm)

target_precompile_headers(MyTextGame PRIVATE "src/Generic.h")

target_sources(MyTextGame PRIVATE "src/MyTextGame.cpp")
target_sources(MyTextGame PRIVATE "src/system/Registry.cpp")

#  Assets
target_sources(MyTextGame PRIVATE "src/assets/Loader.cpp")
target_sources(MyTextGame PRIVATE "src/assets/TextAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/GfxAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/SoundAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/SceneAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/ScriptAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/ModelAsset.cpp")

#   Scripting
target_sources(MyTextGame PRIVATE "src/scripting/Runtime.cpp")

#   Input
target_sources(MyTextGame PRIVATE "src/input/IInput.cpp")
target_sources(MyTextGame PRIVATE "src/input/CameraController.cpp")

#   Render
target_sources(MyTextGame PRIVATE "src/render/Gfx.cpp")
target_sources(MyTextGame PRIVATE "src/render/Camera.cpp")

#   Scene graph
target_sources(MyTextGame PRIVATE "src/entities/Scene.cpp")
target_sources(MyTextGame PRIVATE "src/entities/Node.cpp")
target_sources(MyTextGame PRIVATE "src/entities/Graphic2D.cpp")

#   Audio
target_sources(MyTextGame PRIVATE "src/audio/AudioWrapper.cpp")

#   ImGUI external sources for native implementation.
target_sources(MyTextGame PRIVATE "thirdparty/imgui/imgui.cpp")
target_sources(MyTextGame PRIVATE "thirdparty/imgui/imgui_draw.cpp")
target_sources(MyTextGame PRIVATE "thirdparty/imgui/imgui_tables.cpp")
target_sources(MyTextGame PRIVATE "thirdparty/imgui/imgui_widgets.cpp")
target_sources(MyTextGame PRIVATE "thirdparty/imgui/backends/imgui_impl_sdl3.cpp")
target_sources(MyTextGame PRIVATE "thirdparty/imgui/backends/imgui_impl_sdlrenderer3.cpp")

target_link_libraries(MyTextGame PRIVATE SDL3::SDL3)

target_link_libraries(MyTextGame PRIVATE "jsoncpp_static")

target_link_libraries(MyTextGame PRIVATE fmt::fmt)

target_link_libraries(MyTextGame PRIVATE glm::glm-header-only)

set(CMAKE_BUILD_PARALLEL_LEVEL 10)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MyTextGame PROPERTY CXX_STANDARD 20)
endif()

# Setup testing project.
add_subdirectory("thirdparty/googletest")

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

add_executable(
    MyTextGameTest
    "test/MyTextGameTest.cc"
)

target_link_libraries(
    MyTextGameTest
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(MyTextGameTest)