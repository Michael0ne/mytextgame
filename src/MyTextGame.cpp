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

bool InitGame()
{
    uint32_t assetsLoadedSuccessfully = 0;
    TimerScoped timer([](const TimerDurationType& duration) { std::cout << "InitGame done! Took " << duration << " ms." << std::endl; });

    for (uint32_t i = 0; i < AssetsCount; ++i)
    {
        if (AssetLoaderInstance.OpenAsset(AssetsToLoad[i]))
        {
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
    }

    if (assetsLoadedSuccessfully == AssetsCount)
        return true;
    else
        return false;
}

void UnInitGame()
{
    TimerScoped timer([](const TimerDurationType& duration) { std::cout << "UnInitGame done! Took " << duration << " ms." << std::endl; });

    for (uint32_t i = 0; i < AssetsList.size(); ++i)
    {
        if (AssetsList[i])
            delete AssetsList[i];
    }
}

void LoopGame()
{
    auto timeStart = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> timeInterval(10.0);

    while (true)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);

        const auto timeCurrent = std::chrono::high_resolution_clock::now();
        if ((timeCurrent - timeStart) >= timeInterval)
        {
            std::cout << "10 seconds passed..." << std::endl;
            timeStart = timeCurrent;

            const std::string& title = AssetsList[0]->CastTo<TextAsset>().GetKeyValue("TitleHeader");
            std::cout << title << std::endl;
        }
    }
}

int main(const int argc, const char** argv)
{
    if (InitGame())
        LoopGame();

    UnInitGame();

    return 0;
}