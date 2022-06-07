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
#include <json.h>

#define LOGGER_TAG "[" << __FILE__ << ":" << __func__ << ":" << __LINE__ << "] "