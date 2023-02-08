#include "MyTextGame.h"
#include "Loader.h"
#include "Timer.h"
#include "TextAsset.h"
#include "GfxAsset.h"
#include "SoundAsset.h"
#include "Settings.h"
#include "SceneLoader.h"
#include "KeyboardInput.h"
#include "GamepadInput.h"
#include "Gfx.h"
#include "AssetInterfaceFactory.h"

//  ASSETS
static AssetLoader& AssetLoaderInstance = AssetLoader::GetInstance();
static std::vector<AssetInterface*> AssetsList;
static const std::string dataFileName = "files.dat";

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

bool InstantiateAssets()
{
    AssetLoader::ParseDataFile(dataFileName, AssetsList);

    // return AssetsList.size() > 0;
    return true;
}

uint32_t UnloadAssets()
{
    uint32_t assetsFreed = 0;
    for (uint32_t i = 0; i < AssetsList.size(); ++i)
    {
        if (AssetsList[i])
        {
            assetsFreed++;
            delete AssetsList[i];
        }
    }

    return assetsFreed;
}

void AudioCallback(void* userdata, Uint8* stream, int len)
{
}

bool InitSDL()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
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

    GameWindow = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenResolution[0], ScreenResolution[1], 0);
    if (!GameWindow)
    {
        std::cout << LOGGER_TAG << "Window create error: " << SDL_GetError() << std::endl;
        return false;
    }

    GameWindowSurface = SDL_GetWindowSurface(GameWindow);
    if (!GameWindowSurface)
    {
        std::cout << LOGGER_TAG << "Can't obtain game window surface! " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_AudioSpec DesiredAudioSpec, ActualAudioSpec;
    DesiredAudioSpec.freq = 48100;
    DesiredAudioSpec.format = AUDIO_F32;
    DesiredAudioSpec.channels = 2;
    DesiredAudioSpec.samples = 4096;
    DesiredAudioSpec.callback = AudioCallback;

    GameAudioDeviceId = SDL_OpenAudioDevice(nullptr, 0, &DesiredAudioSpec, &ActualAudioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (!GameAudioDeviceId)
    {
        std::cout << LOGGER_TAG << "Can't obtain audio device! " << SDL_GetError() << std::endl;
        return false;
    }

    GameRenderer = SDL_CreateRenderer(GameWindow, NULL, 0);
    if (!GameRenderer)
    {
        std::cout << LOGGER_TAG << "Can't create renderer! " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void UnInitSDL()
{
    SDL_CloseAudioDevice(GameAudioDeviceId);
    SDL_DestroyRenderer(GameRenderer);
    SDL_DestroyWindow(GameWindow);
    SDL_Quit();

    std::cout << LOGGER_TAG << "Now uninitialized" << std::endl;
}

bool LoadSettings()
{
    Settings::Open("settings.txt");

    return Settings::IsOpen();
}

bool LoadScene()
{
    SceneLoader sceneLoader("assets/menu.scene");
    const Json::Value& menu = sceneLoader.GetSectionValue("menu");

    return sceneLoader.IsOpen();
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
    SDL_SysWMinfo sysWMinfo = {};
    SDL_GetWindowWMInfo(GameWindow, &sysWMinfo, SDL_SYSWM_CURRENT_VERSION);

#ifdef __linux__
    return GfxInstance.Init(sysWMinfo.info.wl.display, ScreenResolution[0], ScreenResolution[1]);
#elif WIN32
    return GfxInstance.Init(sysWMinfo.info.win.window, ScreenResolution[0], ScreenResolution[1]);
#else
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

    if (!InstantiateAssets())
    {
        std::cout << LOGGER_TAG << "InstantiateAssets failed!" << std::endl;
        return false;
    }

    if (!LoadScene())
    {
        std::cout << LOGGER_TAG << "LoadScene failed!" << std::endl;
        return false;
    }

    return true;
}

void UnInitGame()
{
    TimerScoped timer([](const TimerDurationType& duration) { std::cout << LOGGER_TAG << "UnInitGame done! Took " << duration << std::endl; });

    delete InputInstance;
    Settings::Shutdown();
    UnloadAssets();
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
}

void UpdateGfx(SDL_Renderer* renderer, const float delta)
{
    GfxInstance.Update(renderer, delta);
}

void LoopGame()
{
    const Uint64 FrameStartTicks = SDL_GetTicks();

    SDL_SetRenderDrawColor(GameRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(GameRenderer);

    while (SDL_PollEvent(&GameWindowEvent) != 0)
    {
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

    SDL_RenderPresent(GameRenderer);
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