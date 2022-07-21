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
target_include_directories(MyTextGame PRIVATE "src/scene/")
target_include_directories(MyTextGame PRIVATE "src/audio/")
target_include_directories(MyTextGame PRIVATE "src/render/")
target_include_directories(MyTextGame PRIVATE "src/system/")
target_include_directories(MyTextGame PRIVATE "src/input/")
target_include_directories(MyTextGame PRIVATE "thirdparty/xxHash/")
target_include_directories(MyTextGame PRIVATE "thirdparty/SDL/include/")
target_include_directories(MyTextGame PRIVATE "thirdparty/jsoncpp/include/json/")

add_subdirectory("thirdparty/xxHash/cmake_unofficial/")
add_subdirectory("thirdparty/SDL/")
add_subdirectory("thirdparty/jsoncpp/")

target_sources(MyTextGame PRIVATE "src/MyTextGame.cpp")
target_sources(MyTextGame PRIVATE "src/assets/Loader.cpp")
target_sources(MyTextGame PRIVATE "src/assets/TextAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/GfxAsset.cpp")
target_sources(MyTextGame PRIVATE "src/assets/SoundAsset.cpp")
target_sources(MyTextGame PRIVATE "src/scripting/ScriptFunctions.cpp")
target_sources(MyTextGame PRIVATE "src/scene/SceneLoader.cpp")
target_sources(MyTextGame PRIVATE "src/input/IInput.cpp")
target_sources(MyTextGame PRIVATE "src/render/Gfx.cpp")

target_precompile_headers(MyTextGame PRIVATE "src/Generic.h")

target_link_libraries(MyTextGame PRIVATE "xxhash")
target_link_libraries(MyTextGame PRIVATE "SDL2")
target_link_libraries(MyTextGame PRIVATE "jsoncpp_static")

target_link_libraries(MyTextGame PUBLIC "XInput")
target_link_libraries(MyTextGame PUBLIC "dxguid")
target_link_libraries(MyTextGame PUBLIC "dinput8")

set(CMAKE_BUILD_PARALLEL_LEVEL 4)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MyTextGame PROPERTY CXX_STANDARD 20)
endif()

# For SDL2 - after build is finished, copy output DLL into main project's bin directory.
add_custom_command(
    TARGET MyTextGame
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:SDL2>
        $<TARGET_FILE_DIR:MyTextGame>/$<TARGET_FILE_NAME:SDL2>
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