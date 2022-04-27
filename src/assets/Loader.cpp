#include "Loader.h"
#include "AssetInterface.h"

AssetLoader AssetLoader::Instance;

AssetLoader::AssetLoader()
{
    FileOpenStatus = -1;
    FilePtr = nullptr;
    FileDataBuffer = nullptr;
    FileSize = -1;
}

AssetLoader::~AssetLoader()
{
    CloseAsset();
}

bool AssetLoader::CloseAsset()
{
    if (!FilePtr)
        return false;

    fclose(FilePtr);

    FilePtr = nullptr;
    FileOpenStatus = -1;
    delete[] FileDataBuffer;
    FileSize = -1;

    std::cout << "[AssetLoader] File released!" << std::endl;

    return true;
}

const errno_t AssetLoader::GetError() const
{
    return FileOpenStatus;
}

const eAssetType AssetLoader::GetAssetType() const
{
    return AssetType;
}

void AssetLoader::SetAssetRef(AssetInterface* assetInterface)
{
    AssetInterfaceRef = assetInterface;
    assetInterface->SetData(FilePath);
}

bool AssetLoader::OpenAsset(const std::string& path)
{
    if (path.empty())
        return false;

    //  Given path is relative, make it absolute.
    FilePtr = nullptr;

    //  The asset path is relative to the specified asset type.
    //  Check if it is so first.
    const size_t assetTypeStrColonPos = path.find_first_of(':');
    const size_t assetLastSlashPos = path.find_last_of('/');
    if (assetTypeStrColonPos == std::string::npos)
        return false;

    //  Set file name.
    FileName = path.substr((assetLastSlashPos == std::string::npos ? assetTypeStrColonPos : assetLastSlashPos) + 1);

    //  Record what asset type has been requested. Extension is not important.
    std::string assetType = path.substr(0, assetTypeStrColonPos);
    AssetTypeHash = XXH64(assetType.c_str(), assetType.length(), NULL);
    AssetType = (eAssetType)AssetTypeHash;

    const char* assetTypeDirStrPtr = nullptr;
    switch (AssetType)
    {
    case eAssetType::TEXT:
        assetTypeDirStrPtr = ASSET_TEXT_PREFIX;
        break;
    case eAssetType::GFX:
        assetTypeDirStrPtr = ASSET_GFX_PREFIX;
        break;
    case eAssetType::SOUND:
        assetTypeDirStrPtr = ASSET_SOUND_PREFIX;
        break;
    default:
        return false;
    }

    //  Skip first slash if necessary.
    if (assetTypeDirStrPtr[0] == '/' && ASSETS_BASE_DIR[assetsBaseDirLength - 1] == '/')
        assetTypeDirStrPtr++;

    //  Make full path for fopen.
    FilePath = ASSETS_BASE_DIR;
    FilePath += assetTypeDirStrPtr;
    FilePath += path.substr(assetTypeStrColonPos + 1);
    FileOpenStatus = fopen_s(&FilePtr, FilePath.c_str(), "r");

    if (FileOpenStatus)
    {
        std::cout << "[AssetLoader] Can't open \"" << FilePath << "\"!" << std::endl;
        return false;
    }

    //  Allocate buffer large enough.
    fseek(FilePtr, 0l, SEEK_END);
    FileSize = ftell(FilePtr);
    rewind(FilePtr);

    FileDataBuffer = new uint8_t[FileSize];
    memset(FileDataBuffer, NULL, FileSize);
    fread_s(FileDataBuffer, FileSize, FileSize, 1, FilePtr);

    std::cout << "[AssetLoader] Open \"" << FileName << "\" successful, size = " << FileSize << std::endl;

    return true;
}