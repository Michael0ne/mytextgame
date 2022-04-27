#pragma once

#include "Generic.h"

const char* const ASSETS_BASE_DIR = "./assets/";
const char* const ASSET_TEXT_PREFIX = "/text/";
const char* const ASSET_GFX_PREFIX = "/gfx/";
const char* const ASSET_SOUND_PREFIX = "/sounds/";
const size_t assetsBaseDirLength = strlen(ASSETS_BASE_DIR);
const size_t assetTextPrefixLength = strlen(ASSET_TEXT_PREFIX);
const size_t assetGfxPrefixLength = strlen(ASSET_GFX_PREFIX);
const size_t assetSoundPrefixLength = strlen(ASSET_SOUND_PREFIX);

enum class eAssetType : uint64_t
{
    TEXT = 0x80a69b9688ccaf52,  //  Hash for "text".
    GFX = 0x28a480fa8bad468a,    //  Hash for "gfx".
    SOUND = 0x381b96c7a2ec1dff    //  Hash for "sound".
};

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
    XXH64_hash_t    AssetTypeHash;
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
};