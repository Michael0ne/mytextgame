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

set_target_properties(MyTextGame
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY lib
    LIBRARY_OUTPUT_DIRECTORY lib
    RUNTIME_OUTPUT_DIRECTORY bin
)

target_include_directories(MyTextGame PRIVATE "src/")
target_include_directories(MyTextGame PRIVATE "src/assets/")
target_include_directories(MyTextGame PRIVATE "thirdparty/xxHash/")
add_subdirectory("thirdparty/xxHash/cmake_unofficial/")

target_sources(MyTextGame PRIVATE "src/MyTextGame.cpp")
target_sources(MyTextGame PRIVATE "src/assets/Loader.cpp")

target_precompile_headers(MyTextGame PRIVATE "src/Generic.h")

target_link_libraries(MyTextGame PRIVATE "xxhash")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MyTextGame PROPERTY CXX_STANDARD 20)
endif()