#include "MyTextGame.h"
#include "Loader.h"

int main(const int argc, const char** argv)
{
    AssetLoader& alInst = AssetLoader::GetInstance();

    if (!alInst.OpenAsset("text/intro.txt"))
    {
        std::cout << "Failed to open asset file! Error:" << alInst.GetError() << std::endl;
        return 1;
    }

    return 0;
}