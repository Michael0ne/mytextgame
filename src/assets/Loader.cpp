#include "Loader.h"
#include "AssetInterface.h"

AssetLoader AssetLoader::Instance;

AssetLoader::AssetLoader()
{
    FileOpenStatus = -1;
    FilePath = nullptr;
    FilePtr = nullptr;

    AuxInfo.LinesInAssetFile = -1;
    AuxInfo.CommentsInAssetFile = -1;

    std::cout << "AssetLoader constructor!" << std::endl;
}

AssetLoader::~AssetLoader()
{
    CloseAsset();

    std::cout << "AssetLoader destructor!" << std::endl;
}

bool AssetLoader::ParseAssetData(AssetInterface* assetInterface)
{
    //  File is open.
    char buffer[1024] = {};
    AuxInfo.LinesInAssetFile = -1;
    AuxInfo.CommentsInAssetFile = -1;
    //while (fread_s(buffer, sizeof(buffer), sizeof(buffer), 1, FilePtr) != EOF)
    while (std::fgets(buffer, sizeof(buffer), FilePtr))
    {
        if (buffer[0] == '#')
        {
            AuxInfo.CommentsInAssetFile++;
            AuxInfo.LinesInAssetFile++;
            continue;
        }

        std::cout << buffer;
        AuxInfo.LinesInAssetFile++;
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

    FilePtr = nullptr;

    //  The asset path is relative to the specified asset type.
    //  Check if it is so first.
    const char* const assetTypeStrColonPos = strchr(path, ':');
    if (!assetTypeStrColonPos)
        return false;

    //  Record what asset type has been requested. Extension is not important.
    char assetTypeStr[32] = {};
    const size_t assetTypeStrLength = assetTypeStrColonPos - path;
    const size_t assetPathStrLength = strlen(path) - assetTypeStrLength;
    strncpy_s(assetTypeStr, assetTypeStrLength + 1, path, assetTypeStrLength);
    AssetTypeHash = XXH64(assetTypeStr, assetTypeStrLength, NULL);
    AssetType = (eAssetType)AssetTypeHash;

    const char* assetTypeDirStrPtr = nullptr;
    size_t assetTypeDirStrLength = 0;
    switch (AssetType)
    {
    case eAssetType::TEXT:
        assetTypeDirStrPtr = ASSET_TEXT_PREFIX;
        assetTypeDirStrLength = assetTextPrefixLength;
        break;
    case eAssetType::GFX:
        assetTypeDirStrPtr = ASSET_GFX_PREFIX;
        assetTypeDirStrLength = assetGfxPrefixLength;
        break;
    case eAssetType::SOUND:
        assetTypeDirStrPtr = ASSET_SOUND_PREFIX;
        assetTypeDirStrLength = assetSoundPrefixLength;
        break;
    default:
        return false;
    }

    //  Skip first slash if necessary.
    if (assetTypeDirStrPtr[0] == '/' && ASSETS_BASE_DIR[assetsBaseDirLength - 1] == '/')
    {
        assetTypeDirStrPtr++;
        assetTypeDirStrLength--;
    }

    //  Make full path for fopen.
    const size_t FilePathStringLength = assetsBaseDirLength + assetTypeDirStrLength + assetPathStrLength + 1;
    FilePath = new char[FilePathStringLength];
    strcpy_s(FilePath, FilePathStringLength, ASSETS_BASE_DIR);
    strcat_s(FilePath, FilePathStringLength, assetTypeDirStrPtr);
    strcat_s(FilePath, FilePathStringLength, assetTypeStrColonPos + 1);
    FileOpenStatus = fopen_s(&FilePtr, FilePath, "r");

    if (FileOpenStatus)
        return false;

    std::cout << "OpenAsset(" << FilePath << "): FilePtr = " << std::hex << FilePtr << std::endl;

    return ParseAssetData(AssetInterfaceRef);
}

AssetLoader& AssetLoader::GetInstance()
{
    return Instance;
}