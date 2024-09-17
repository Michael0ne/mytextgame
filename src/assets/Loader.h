#pragma once

#include "Generic.h"
#include "AssetInterface.h"
#include "Logger.h"

//  When adding a new type of 'asset' don't forget to put it into 'eAssetType' enumeration, but also into 'AssetPathPrefix'.
//  Also, don't forget to modify the 'switch' statement in AssetInterfaceFactory to account for your new AssetType.
enum class eAssetType : HashType
{
    TEXT = xxh64::hash("text", 4, 0),
    GFX = xxh64::hash("gfx", 3, 0),
    SOUND = xxh64::hash("sound", 5, 0),
    SCRIPT = xxh64::hash("script", 6, 0),
    SCENE = xxh64::hash("scene", 5, 0),
    MODEL = xxh64::hash("model", 5, 0)
};

static const std::unordered_map<eAssetType, std::string> AssetPathPrefix =
{
    { eAssetType::TEXT, "text/" },
    { eAssetType::GFX,  "gfx/" },
    { eAssetType::SOUND, "sound/" },
    { eAssetType::SCENE, "scenes/" },
    { eAssetType::SCRIPT, "scripts/" },
    { eAssetType::MODEL, "models/" }
};

static const std::string AssetBaseDir = "./assets/";

class AssetLoader
{
private:
    FileErrorType   FileOpenStatus;
    std::string     FilePath;
    std::string     FileName;
    FILE           *FilePtr;
    uint8_t        *FileDataBuffer;
    long            FileSize;

    eAssetType      AssetType;
    HashType        AssetTypeHash;
    AssetInterface *AssetInterfaceRef;

    AssetLoader();
    ~AssetLoader();

    static AssetLoader  Instance;

public:
    const FileErrorType GetError() const;
    const eAssetType    GetAssetType() const;
    inline const uint8_t* GetDataBufferPtr() const
    {
        return FileDataBuffer;
    }

    void            SetAssetRef(AssetInterface* assetInterface);

    bool            OpenAsset(const std::string& path);
    bool            CloseAsset();

    static std::vector<AssetInterface*>     Assets;

    static inline void Shutdown()
    {
        uint32_t assetsFreed = 0;
        for (uint32_t i = 0; i < Assets.size(); ++i)
        {
            if (Assets[i])
            {
                assetsFreed++;
                delete Assets[i];
            }
        }

        Logger::TRACE(TAG_FUNCTION_NAME, "Unloaded {} assets.", assetsFreed);
    }

    static inline AssetLoader& GetInstance()
    {
        return Instance;
    }

    //  Given input path with format '<asset type>:<folder>/<filename>.<extension>' this will return parsed parts of it.
    static void ParsePath(
        const std::string_view& path,
        std::string& fileName,
        std::string& fileExtension,
        std::string& folderPath,
        eAssetType& fileType)
    {
        const size_t typeSeparatorPosition = path.find_first_of(':');
        const size_t filenameStartPosition = path.find_last_of('/');
        const size_t filenameExtensionPosition = path.find_last_of('.');

        //  Can't find at least one part that should be in the input - bail out!
        //  TODO: maybe throw or return false?
        if (typeSeparatorPosition == std::string::npos ||
            filenameStartPosition == std::string::npos ||
            filenameExtensionPosition == std::string::npos)
        {
            Logger::ERROR(TAG_FUNCTION_NAME, "Malformed path passed! Can't find required characters (: . /).");
            return;
        }

        const HashType typeHash = xxh64::hash(path.data(), path.size(), 0);
        fileName = path.substr(filenameStartPosition + 1, filenameExtensionPosition - filenameStartPosition - 1);
        fileExtension = path.substr(filenameExtensionPosition + 1);
        folderPath = path.substr(typeSeparatorPosition + 1, filenameStartPosition - typeSeparatorPosition - 1);
        fileType = (eAssetType)typeHash;
    }

    //  Open data file and instantiate all assets that are within.
    static bool ParseDataFile(const std::string dataFilePath);
};