#include "Loader.h"

AssetLoader AssetLoader::Instance;

AssetLoader::AssetLoader()
{
    FileOpenStatus = -1;
    FilePath = nullptr;
    FilePtr = nullptr;

    LinesInAssetFile = -1;

    std::cout << "AssetLoader constructor!" << std::endl;
}

AssetLoader::~AssetLoader()
{
    CloseAsset();

    std::cout << "AssetLoader destructor!" << std::endl;
}

bool AssetLoader::ParseAssetData()
{
    //  File is open.
    //  Read data and put where it needs to go.
    char buffer[1024] = {};
    LinesInAssetFile = -1;
    while (fread_s(buffer, sizeof(buffer), sizeof(buffer), 1, FilePtr) != EOF)
    {
        std::cout << buffer << std::endl;
        LinesInAssetFile++;
    }

    return true;
}

bool AssetLoader::CloseAsset()
{
    if (!FilePtr)
        return false;

    fclose(FilePtr);

    FilePath = nullptr;
    FilePtr = nullptr;
    FileOpenStatus = -1;

    return true;
}

const errno_t AssetLoader::GetError() const
{
    return FileOpenStatus;
}

bool AssetLoader::OpenAsset(const char* const path)
{
    if (!path)
        return false;

    //  Given path is relative, make it absolute.
    if (FilePath)
        delete[] FilePath;

    const size_t pathSize = strlen(path) + 1;
    FilePath = new char[8 + pathSize];
    if (!FilePath)
        return false;

    strcpy_s(FilePath, 9, ASSETS_BASE_DIR);
    strcat_s(FilePath, pathSize + 8, path);

    FilePtr = nullptr;
    FileOpenStatus = fopen_s(&FilePtr, FilePath, "r");

    if (FileOpenStatus)
        return false;

    std::cout << "OpenAsset(" << FilePath << "): FilePtr = " << std::hex << FilePtr << std::endl;

    return ParseAssetData();
}

AssetLoader& AssetLoader::GetInstance()
{
    return Instance;
}