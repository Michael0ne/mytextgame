﻿#include "MyTextGame.h"
#include "Loader.h"
#include "Timer.h"
#include "TextAsset.h"
#include "GfxAsset.h"
#include "SoundAsset.h"

static AssetLoader& AssetLoaderInstance = AssetLoader::GetInstance();
static std::vector<AssetInterface*> AssetsList;
static const std::list<std::string> AssetsToLoad =
{
    //  Text
    "text:intro.txt",
    "text:menumain.txt",
    //  Graphics
    "gfx:title/bg.jpg",
    "gfx:title/gametitle.jpg"
};

static SDL_Window* GameWindow = nullptr;
static SDL_Surface* GameWindowSurface = nullptr;
static SDL_Event GameWindowEvent;
static bool QuitRequested = false;
static SDL_Renderer* GameRenderer;
static SDL_AudioDeviceID GameAudioDeviceId;

uint32_t InstantiateAssets()
{
    uint32_t assetsLoadedSuccessfully = 0;
    for (auto it = AssetsToLoad.begin(); it != AssetsToLoad.end(); ++it)
    {
        if (!AssetLoaderInstance.OpenAsset(*it))
            continue;

        assetsLoadedSuccessfully++;
        AssetInterface* assetInterface = nullptr;
        switch (AssetLoaderInstance.GetAssetType())
        {
        case eAssetType::TEXT:
            assetInterface = new TextAsset();
            break;
        case eAssetType::GFX:
            assetInterface = new GfxAsset();
            break;
        case eAssetType::SOUND:
            assetInterface = new SoundAsset();
            break;
        }

        AssetLoaderInstance.SetAssetRef(assetInterface);
        assetInterface->ParseData(AssetLoaderInstance.GetDataBufferPtr());

        AssetsList.push_back(assetInterface);
        AssetLoaderInstance.CloseAsset();
    }

    return assetsLoadedSuccessfully;
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
        std::cout << "[SDL] Init error: " << SDL_GetError() << std::endl;
        return false;
    }

    GameWindow = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
    if (!GameWindow)
    {
        std::cout << "[SDL] Window create error: " << SDL_GetError() << std::endl;
        return false;
    }

    GameWindowSurface = SDL_GetWindowSurface(GameWindow);
    if (!GameWindowSurface)
    {
        std::cout << "[SDL] Can't obtain game window surface! " << SDL_GetError() << std::endl;
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
        std::cout << "[SDL] Can't obtain audio device! " << SDL_GetError() << std::endl;
        return false;
    }

    GameRenderer = SDL_CreateRenderer(GameWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!GameRenderer)
    {
        std::cout << "[SDL] Can't create renderer! " << SDL_GetError() << std::endl;
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

    std::cout << "[SDL] Now uninitialized" << std::endl;
}

bool InitGame()
{
    TimerScoped timer([](const TimerDurationType& duration) { std::cout << "InitGame done! Took " << duration << std::endl; });

    const bool sdlReady = InitSDL();
    const uint32_t assetsLoaded = InstantiateAssets();
    std::cout << "[InitGame] Assets loaded (" << assetsLoaded << "/" << AssetsToLoad.size() << ")" << std::endl;

    if (assetsLoaded == AssetsToLoad.size() && sdlReady)
        return true;
    else
        return false;
}

void UnInitGame()
{
    TimerScoped timer([](const TimerDurationType& duration) { std::cout << "UnInitGame done! Took " << duration << std::endl; });

    UnloadAssets();
    UnInitSDL();
}

void LoopGame()
{
    while (SDL_PollEvent(&GameWindowEvent) != 0)
    {
        if (GameWindowEvent.type == SDL_QUIT)
            QuitRequested = true;

        if (GameWindowEvent.type == SDL_KEYDOWN)
        {
            switch (GameWindowEvent.key.keysym.sym)
            {
            case SDLK_UP:
                SDL_PauseAudioDevice(GameAudioDeviceId, 0);
                break;
            case SDLK_DOWN:
                SDL_PauseAudioDevice(GameAudioDeviceId, 1);
                break;
            }
        }

        SDL_SetRenderDrawColor(GameRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(GameRenderer);

        SDL_RenderPresent(GameRenderer);

        if (QuitRequested)
            break;
    };
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