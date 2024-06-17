#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <xxh64.hpp>
#include <chrono>
#include <thread>
#include <exception>
#include <stdio.h>
#include <list>

#define SDL_MAIN_HANDLED

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>

#ifdef __linux__
#include <valgrind/valgrind.h>

#elif _WIN32
#define _AMD64_
#include <windef.h>

#define SDL_WINDOWS
#endif

#include <json.h>

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

#include <fmt/core.h>
#include <fmt/color.h>

using HashType = uint64_t;
using FileErrorType = int32_t;
#ifdef __linux__
using WindowHandle = wl_display*;
#elif WIN32
using WindowHandle = HWND;
#endif
using KeyCodeType = SDL_Scancode;