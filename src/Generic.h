#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <xxhash.h>
#include <chrono>
#include <thread>

#define _AMD64_
#include <windef.h>

#define SDL_MAIN_HANDLED
#define SDL_ENABLE_SYSWM_WINDOWS
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_syswm.h>
#include <json.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

using WindowHandle = HWND;

#define LOGGER_TAG "[" << __FILE__ << ":" << __func__ << ":" << __LINE__ << "] "