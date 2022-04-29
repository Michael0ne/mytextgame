#include "MyTextGame.h"
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

bool InitGame()
{
    TimerScoped timer([](const TimerDurationType& duration) { std::cout << "InitGame done! Took " << duration << std::endl; });

    const uint32_t assetsLoaded = InstantiateAssets();
    std::cout << "Assets loaded (" << assetsLoaded << "/" << AssetsToLoad.size() << ")" << std::endl;

    if (assetsLoaded == AssetsToLoad.size())
        return true;
    else
        return false;
}

void UnInitGame()
{
    TimerScoped timer([](const TimerDurationType& duration) { std::cout << "UnInitGame done! Took " << duration << std::endl; });

    for (uint32_t i = 0; i < AssetsList.size(); ++i)
    {
        if (AssetsList[i])
            delete AssetsList[i];
    }
}

void ProcessInput()
{
    std::cout << "ProcessInput" << std::endl;

    while (true)
        _sleep(100);
}

void ProcessLogic()
{
    std::cout << "ProcessLogic" << std::endl;

    while (true)
        _sleep(100);
}

void ProcessGfx()
{
    std::cout << "ProcessGfx" << std::endl;

    while (true)
        _sleep(100);
}

void LoopGame()
{
    std::thread processInputThread(&ProcessInput);
    std::thread processLogicThread(&ProcessLogic);
    std::thread processGfxThread(&ProcessGfx);

    processInputThread.join();
    processLogicThread.join();
    processGfxThread.join();
}

int main(const int argc, const char** argv)
{
    if (InitGame())
        LoopGame();

    UnInitGame();

    return 0;
}