#pragma once

#include "Generic.h"

enum class eAssetType : HashType
{
    TEXT = xxh64::hash("text", 4, 0),
    GFX = xxh64::hash("gfx", 3, 0),
    SOUND = xxh64::hash("sound", 5, 0),
    SCRIPT = xxh64::hash("script", 6, 0)
    //TEXT = 0x80a69b9688ccaf52,      //  Hash for "text".
    //GFX = 0x28a480fa8bad468a,       //  Hash for "gfx".
    //SOUND = 0x381b96c7a2ec1dff,     //  Hash for "sound".
    //SCRIPT = 0xcf7e685ca7386f66     //  Hash for "script".
};

static const std::unordered_map<eAssetType, std::string> AssetPathPrefix =
{
    { eAssetType::TEXT, "text/" },
    { eAssetType::GFX,  "gfx/" },
    { eAssetType::SOUND, "sound/" }
};

static const std::string AssetBaseDir = "./assets/";

class AssetInterface;

class AssetLoader
{
    errno_t     FileOpenStatus;
    std::string FilePath;
    std::string FileName;
    FILE       *FilePtr;
    uint8_t    *FileDataBuffer;
    long        FileSize;

    eAssetType      AssetType;
    HashType        AssetTypeHash;
    AssetInterface *AssetInterfaceRef;

    AssetLoader();
    ~AssetLoader();

    static AssetLoader  Instance;

public:
    const errno_t   GetError() const;
    const eAssetType    GetAssetType() const;
    inline const uint8_t* GetDataBufferPtr() const
    {
        return FileDataBuffer;
    }

    void            SetAssetRef(AssetInterface* assetInterface);

    bool            OpenAsset(const std::string& path);
    bool            CloseAsset();

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
            return;

        const HashType typeHash = xxh64::hash(path.data(), path.size(), 0);
        fileName = path.substr(filenameStartPosition + 1, filenameExtensionPosition - filenameStartPosition - 1);
        fileExtension = path.substr(filenameExtensionPosition + 1);
        folderPath = path.substr(typeSeparatorPosition + 1, filenameStartPosition - typeSeparatorPosition - 1);
        fileType = (eAssetType)typeHash;
    }

    //  Open data file and instantiate all assets that are within.
    static void ParseDataFile(const std::string dataFilePath, std::vector<AssetInterface*>& assets);
};