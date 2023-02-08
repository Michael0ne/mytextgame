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

#include <SDL3/SDL_syswm.h>
#include <json.h>

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

using HashType = uint64_t;
using FileErrorType = int32_t;
using WindowHandle = wl_display*;
using KeyCodeType = SDL_Scancode;

#define LOGGER_TAG "[" << __FILE__ << ":" << __func__ << ":" << __LINE__ << "] "