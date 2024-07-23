#include "MyTextGame.h"
#include "Loader.h"
#include "Timer.h"
#include "TextAsset.h"
#include "GfxAsset.h"
#include "SoundAsset.h"
#include "Settings.h"
#include "KeyboardInput.h"
#include "GamepadInput.h"
#include "Gfx.h"
#include "Logger.h"
#include "AssetInterfaceFactory.h"
#include "scripting/Runtime.h"

//  ASSETS
static AssetLoader& AssetLoaderInstance = AssetLoader::GetInstance();
static const std::string dataFileName = "startup.dat";

//  SDL
static SDL_Window* GameWindow = nullptr;
static SDL_Surface* GameWindowSurface = nullptr;
static SDL_Event GameWindowEvent;
static bool QuitRequested = false;
static SDL_Renderer* GameRenderer;
static SDL_AudioDeviceID GameAudioDeviceId;

//  INPUT
static InputInterface* InputInstance = nullptr;

//  GFX
static Gfx& GfxInstance = Gfx::GetInstance();
static uint32_t ScreenResolution[2] = { 800, 600 };

//  DebugUI
namespace DebugUI
{
    extern bool Init(SDL_Window* SDLWindow, SDL_Renderer* SDLRenderer);
    extern void Update(const float_t delta);
    extern void UnInit();
}

//  ImGUI
extern bool ImGui_ImplSDL3_ProcessEvent(const SDL_Event*);

void SDLCALL AudioCallBack(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount)
{

}

bool InitSDL()
{
    constexpr auto SDLInitFlags = SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS;

    if (SDL_Init(SDLInitFlags) < 0)
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "Init error : {}", SDL_GetError());
        return false;
    }

    const auto widthCustom = Settings::GetValue<uint32_t>("width", 0);
    const auto heightCustom = Settings::GetValue<uint32_t>("height", 0);
    if (widthCustom > 0 && heightCustom > 0)
    {
        ScreenResolution[0] = widthCustom;
        ScreenResolution[1] = heightCustom;
    }

    SDL_PropertiesID windowProperties = SDL_CreateProperties();
    SDL_SetStringProperty(windowProperties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Application");
    SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, ScreenResolution[0]);
    SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, ScreenResolution[1]);
    SDL_GetNumberProperty(windowProperties, "flags", SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    GameWindow = SDL_CreateWindowWithProperties(windowProperties);
    SDL_DestroyProperties(windowProperties);
    if (!GameWindow)
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "Window create error: {}", SDL_GetError());
        return false;
    }

    const SDL_AudioSpec spec = { SDL_AUDIO_S32, 2, 44100 };
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, AudioCallBack, nullptr);
    if (!stream)
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "Can't obtain audio device! {}", SDL_GetError());
        return false;
    }

    GameRenderer = SDL_CreateRenderer(GameWindow, NULL);
    if (!GameRenderer)
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "Can't create renderer! {}", SDL_GetError());
        return false;
    }

    GameWindowSurface = SDL_GetWindowSurface(GameWindow);
    if (!GameWindowSurface)
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "Can't obtain game window surface! {}", SDL_GetError());
        return false;
    }

    if (!DebugUI::Init(GameWindow, GameRenderer))
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "Can't init DebugUI!");
        return false;
    }

    return true;
}

void UnInitSDL()
{
    //SDL_CloseAudioDevice(GameAudioDeviceId);
    SDL_CloseAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
    SDL_DestroyRenderer(GameRenderer);
    SDL_DestroyWindow(GameWindow);
    SDL_Quit();

    Logger::TRACE(TAG_FUNCTION_NAME, "Now uninitialized");
}

bool LoadSettings()
{
    Settings::Open("settings.txt");

    if (!Settings::IsOpen())
        return false;

    SceneAsset::ActiveScene = Settings::GetValue<std::string>("scene", "");

    return true;
}

bool InitInput()
{
    const std::string inputTypeSetting = Settings::GetValue<std::string>("input", "keyboard");
    const HashType inputTypeSettingHash = xxh64::hash(inputTypeSetting.c_str(), inputTypeSetting.length(), 0);

    Logger::TRACE(TAG_FUNCTION_NAME, "Set input type to {}", inputTypeSetting);

    InputInstance = new InputInterface((eInputType)inputTypeSettingHash, GfxInstance.GetWindowHandle());
    return InputInstance->IsValid();
}

bool InitGfx()
{
#ifdef __linux__
    const
        char* videoDriver = SDL_GetCurrentVideoDriver(),
        bool isX11 = (videoDriver[0] == 'x' && videoDriver[1] == '1' && videoDriver[2] == '1'),
        bool isWayland = (videoDriver[0] == 'w' && videoDriver[1] == 'a');

    if (isX11)
    {
        Display* xdisplay = (Display*)SDL_GetProperty(SDL_GetWindowProperties(GameWindow), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
        Window xwindow = (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(GameWindow), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        if (xdisplay && xwindow) {
            Logger::TRACE(TAG_FUNCTION_NAME, "Initializing Gfx for X11...");

            return GfxInstance.Init(xwindow, ScreenResolution[0], ScreenResolution[1]);
        }

        return false;
    }

    if (isWayland)
    {
        struct wl_display* display = (struct wl_display*)SDL_GetProperty(SDL_GetWindowProperties(GameWindow), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
        struct wl_surface* surface = (struct wl_surface*)SDL_GetProperty(SDL_GetWindowProperties(GameWindow), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
        HWND wlndWindow = NULL; //  TODO: window handle for wayland?
        if (display && surface) {
            Logger::TRACE(TAG_FUNCTION_NAME, "Initializing Gfx for Wayland...");

            return GfxInstance.Init(wlndWindow, ScreenResolution[0], ScreenResolution[1]);
        }

        return false;
    }

    return false;

#elif WIN32
    const HWND windowsHWND = (HWND)SDL_GetProperty(SDL_GetWindowProperties(GameWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);

    Logger::TRACE(TAG_FUNCTION_NAME, "Initializing Gfx for Win32...");

    return GfxInstance.Init(windowsHWND, ScreenResolution[0], ScreenResolution[1]);
#else

#pragma error("Gfx environment is not selected!");
    return false;
#endif
}

bool InitGame()
{
    TimerScoped timer([](const TimerDurationType& duration) { Logger::TRACE(TAG_FUNCTION_NAME, "InitGame done! Took "); });

    if (!LoadSettings())
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "LoadSettings failed!");
        return false;
    }

    if (!InitSDL())
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "InitSDL failed!");
        return false;
    }

    if (!InitGfx())
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "InitGfx failed!");
        return false;
    }

    if (!InitInput())
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "InitInput failed!");
        return false;
    }

    if (!AssetLoader::ParseDataFile(dataFileName))
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "InstantiateAssets failed!");
        return false;
    }

    if (!Settings::GetValue<bool>("scripts", true) || !Scripting::Runtime::Start())
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "Scripting::Runtime::Start failed!");
        return false;
    }

    return true;
}

void UnInitGame()
{
    TimerScoped timer([](const TimerDurationType& duration) { Logger::TRACE(TAG_FUNCTION_NAME, "UnInitGame done! Took {}", duration.count()); });

    DebugUI::UnInit();
    Scripting::Runtime::Stop();
    delete InputInstance;
    Settings::Shutdown();
    AssetLoader::Shutdown();
    UnInitSDL();
}

void UpdateInput()
{
    InputInstance->Update();
}

void UpdateLogic(const float delta)
{
    //  ESCAPE to exit application.
    if (InputInstance->KeyPressed(SDL_SCANCODE_ESCAPE))
        QuitRequested = true;

    Scripting::Runtime::Update(delta);
}

void UpdateGfx(SDL_Renderer* renderer, const float delta)
{
    GfxInstance.Update(renderer, delta);
}

void LoopGame()
{
    const Uint64 FrameStartTicks = SDL_GetTicks();

    while (SDL_PollEvent(&GameWindowEvent) != 0)
    {
        ImGui_ImplSDL3_ProcessEvent(&GameWindowEvent);
        switch (GameWindowEvent.type)
        {
        case SDL_EVENT_QUIT:
            QuitRequested = true;
            break;
        }
    };

    const float FrameDelta = (SDL_GetTicks() - FrameStartTicks) * 0.0001f;

    UpdateInput();
    UpdateLogic(FrameDelta);
    UpdateGfx(GameRenderer, FrameDelta);
}

int main(const int argc, const char** argv)
{
    Logger::TRACE(TAG_FUNCTION_NAME, "Begin game init...");

    if (InitGame())
    {
        while (!QuitRequested)
            LoopGame();
    }

    Logger::TRACE(TAG_FUNCTION_NAME, "Exit requested. Terminating...");

    UnInitGame();

    Logger::TRACE(TAG_FUNCTION_NAME, "Game uninit done.");

    return 0;
}