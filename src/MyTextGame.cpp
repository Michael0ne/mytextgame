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
#include "AssetInterfaceFactory.h"
#include "ScriptFunctions.h"

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
        std::cout << LOGGER_TAG << "Init error : " << SDL_GetError() << std::endl;
        return false;
    }

    const uint32_t widthCustom = atoi(Settings::GetValue("width", "").c_str());
    const uint32_t heightCustom = atoi(Settings::GetValue("height", "").c_str());
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
        std::cout << LOGGER_TAG << "Window create error: " << SDL_GetError() << std::endl;
        return false;
    }

    const SDL_AudioSpec spec = { SDL_AUDIO_S32, 2, 44100 };
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_OUTPUT, &spec, AudioCallBack, nullptr);
    if (!stream)
    {
        std::cout << LOGGER_TAG << "Can't obtain audio device! " << SDL_GetError() << std::endl;
        return false;
    }

    GameRenderer = SDL_CreateRenderer(GameWindow, NULL, SDL_RENDERER_ACCELERATED);
    if (!GameRenderer)
    {
        std::cout << LOGGER_TAG << "Can't create renderer! " << SDL_GetError() << std::endl;
        return false;
    }

    GameWindowSurface = SDL_GetWindowSurface(GameWindow);
    if (!GameWindowSurface)
    {
        std::cout << LOGGER_TAG << "Can't obtain game window surface! " << SDL_GetError() << std::endl;
        return false;
    }

    if (!DebugUI::Init(GameWindow, GameRenderer))
    {
        std::cout << LOGGER_TAG << "Can't init DebugUI!" << std::endl;
        return false;
    }

    return true;
}

void UnInitSDL()
{
    //SDL_CloseAudioDevice(GameAudioDeviceId);
    SDL_CloseAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_OUTPUT);
    SDL_DestroyRenderer(GameRenderer);
    SDL_DestroyWindow(GameWindow);
    SDL_Quit();

    std::cout << LOGGER_TAG << "Now uninitialized" << std::endl;
}

bool LoadSettings()
{
    Settings::Open("settings.txt");

    if (!Settings::IsOpen())
        return false;

    SceneAsset::ActiveScene = Settings::GetValue("scene", std::string());

    return true;
}

bool InitInput()
{
    const std::string inputTypeSetting = Settings::GetValue("input", "keyboard");
    const HashType inputTypeSettingHash = xxh64::hash(inputTypeSetting.c_str(), inputTypeSetting.length(), 0);

    std::cout << LOGGER_TAG << "InputType: " << inputTypeSetting << std::endl;

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
            std::cout << LOGGER_TAG << "Initializing Gfx for X11..." << std::endl;

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
            std::cout << LOGGER_TAG << "Initializing Gfx for Wayland..." << std::endl;

            return GfxInstance.Init(wlndWindow, ScreenResolution[0], ScreenResolution[1]);
        }

        return false;
    }

    return false;

#elif WIN32
    const HWND windowsHWND = (HWND)SDL_GetProperty(SDL_GetWindowProperties(GameWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);

    std::cout << LOGGER_TAG << "Initializing Gfx for Win32..." << std::endl;

    return GfxInstance.Init(windowsHWND, ScreenResolution[0], ScreenResolution[1]);
#else

#pragma error("Gfx environment is not selected!");
    return false;
#endif
}

bool InitGame()
{
    TimerScoped timer([](const TimerDurationType& duration) { std::cout << LOGGER_TAG << "InitGame done! Took " << duration << std::endl; });

    if (!LoadSettings())
    {
        std::cout << LOGGER_TAG << "LoadSettings failed!" << std::endl;
        return false;
    }

    if (!InitSDL())
    {
        std::cout << LOGGER_TAG << "InitSDL failed!" << std::endl;
        return false;
    }

    if (!InitGfx())
    {
        std::cout << LOGGER_TAG << "InitGfx failed!" << std::endl;
        return false;
    }

    if (!InitInput())
    {
        std::cout << LOGGER_TAG << "InitInput failed!" << std::endl;
        return false;
    }

    if (!AssetLoader::ParseDataFile(dataFileName))
    {
        std::cout << LOGGER_TAG << "InstantiateAssets failed!" << std::endl;
        return false;
    }

    if (!ScriptEngine::Start())
    {
        std::cout << LOGGER_TAG << "ScriptEngine::Start failed!" << std::endl;
        return false;
    }

    return true;
}

void UnInitGame()
{
    TimerScoped timer([](const TimerDurationType& duration) { std::cout << LOGGER_TAG << "UnInitGame done! Took " << duration << std::endl; });

    DebugUI::UnInit();
    ScriptEngine::Stop();
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

    ScriptEngine::Update(delta);
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
    if (InitGame())
    {
        while (!QuitRequested)
            LoopGame();
    }

    UnInitGame();

    return 0;
}