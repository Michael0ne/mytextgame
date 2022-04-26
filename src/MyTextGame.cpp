#include "MyTextGame.h"
#include "Loader.h"
#include "Timer.h"

static AssetLoader& alInst = AssetLoader::GetInstance();

void ReportInitTimings(TimerScoped::TimerDurationType& duration)
{
    std::cout << "InitGame done! Took " << duration << " ms." << std::endl;
}

void InitGame()
{
    TimerScoped timer(ReportInitTimings);

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