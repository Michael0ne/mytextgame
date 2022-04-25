#include "MyTextGame.h"
#include "Loader.h"

static AssetLoader& alInst = AssetLoader::GetInstance();

void InitGame()
{
    alInst.OpenAsset("text:intro.txt");
    alInst.OpenAsset("text:menumain.txt");
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

    return 0;
}