#pragma once
#define SDL_MAIN_HANDLED

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <xxhash.h>
#include <chrono>
#include <thread>
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_syswm.h>
#include <json.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

using WindowHandle = HWND;

#define LOGGER_TAG "[" << __FILE__ << ":" << __func__ << ":" << __LINE__ << "] "