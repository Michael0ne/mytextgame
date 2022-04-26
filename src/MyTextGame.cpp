#include "MyTextGame.h"
#include "Loader.h"
#include "Timer.h"
#include "TextAsset.h"
#include "GfxAsset.h"
#include "SoundAsset.h"

static AssetLoader& AssetLoaderInstance = AssetLoader::GetInstance();
static std::vector<AssetInterface*> AssetsList;
static const char* AssetsToLoad[] =
{
    //  Text
    "text:intro.txt",
    "text:menumain.txt",
    //  Graphics
    "gfx:title/bg.jpg",
    "gfx:title/gametitle.jpg"
};
static constexpr size_t AssetsCount = 4;

void InitGame()
{
    uint32_t assetsLoadedSuccessfully = 0;
    TimerScoped timer([](TimerScoped::TimerDurationType& duration) { std::cout << "InitGame done! Took " << duration << " ms." << std::endl; });

    for (uint32_t i = 0; i < AssetsCount; ++i)
    {
        if (AssetLoaderInstance.OpenAsset(AssetsToLoad[i]))
        {
            assetsLoadedSuccessfully++;

            switch (AssetLoaderInstance.GetAssetType())
            {
            case eAssetType::TEXT:
                AssetsList.push_back(new TextAsset(nullptr));
                break;
            case eAssetType::GFX:
                AssetsList.push_back(new GfxAsset(nullptr));
                break;
            case eAssetType::SOUND:
                AssetsList.push_back(new SoundAsset(nullptr));
                break;
            }

            AssetLoaderInstance.CloseAsset();
        }
    }

    if (assetsLoadedSuccessfully == AssetsCount)
        std::cout << "Assets loaded!" << std::endl;
}

void UnInitGame()
{
    TimerScoped timer([](TimerScoped::TimerDurationType& duration) { std::cout << "UnInitGame done! Took " << duration << " ms." << std::endl; });

    for (uint32_t i = 0; i < AssetsList.size(); ++i)
    {
        if (AssetsList[i])
            delete AssetsList[i];
    }
}

void LoopGame()
{
    while (true)
        _sleep(10);
}

int main(const int argc, const char** argv)
{
    InitGame();
    LoopGame();
    UnInitGame();

    return 0;
}