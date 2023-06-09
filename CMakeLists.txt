# CMakeList.txt : CMake project for MyTextGame, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("MyTextGame")

# Add source to this project's executable.
add_executable (MyTextGame "src/MyTextGame.cpp" "src/MyTextGame.h")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

set(JSONCPP_WITH_TESTS OFF)
set(JSONCPP_WITH_EXAMPLE OFF)

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
target_include_directories(MyTextGame PRIVATE "thirdparty/xxhashct")
target_include_directories(MyTextGame PRIVATE "thirdparty/SDL/include/")
target_include_directories(MyTextGame PRIVATE "thirdparty/jsoncpp/include/json/")
target_include_directories(MyTextGame PRIVATE "thirdparty/imgui/")

add_subdirectory("thirdparty/SDL/")
add_subdirectory("thirdparty/jsoncpp/")

target_precompile_headers(MyTextGame PRIVATE "src/Generic.h")

target_sources(MyTextGame PRIVATE "src/MyTextGame.cpp")

#  Assets
target_sources(MyTextGame PRIVATE "src/assets/Loader.cpp")
target_sources(MyTextGame PRIVATE "src/assets/TextAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/GfxAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/SoundAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/SceneAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/ScriptAsset.cpp")

#   Scripting
target_sources(MyTextGame PRIVATE "src/scripting/ScriptFunctions.cpp")

#   Input
target_sources(MyTextGame PRIVATE "src/input/IInput.cpp")

#   Render
target_sources(MyTextGame PRIVATE "src/render/Gfx.cpp")

if (TARGET SDL3::SDL3_main)
    target_link_libraries(MyTextGame PRIVATE SDL3::SDL3_main)
endif()
target_link_libraries(MyTextGame PRIVATE SDL3::SDL3)

target_link_libraries(MyTextGame PRIVATE "jsoncpp_static")

set(CMAKE_BUILD_PARALLEL_LEVEL 4)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MyTextGame PROPERTY CXX_STANDARD 20)
endif()

# For SDL - after build is finished, copy output DLL into main project's bin directory.
add_custom_command(
    TARGET MyTextGame
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:SDL3>
        $<TARGET_FILE_DIR:MyTextGame>/$<TARGET_FILE_NAME:SDL3>
)

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