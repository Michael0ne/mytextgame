#include "Generic.h"

#define ASSETS_BASE_DIR "/assets/"

class AssetLoader
{
private:
    errno_t     FileOpenStatus;
    char       *FilePath;
    FILE       *FilePtr;

    int32_t     LinesInAssetFile;

    AssetLoader();
    ~AssetLoader();

    bool        ParseAssetData();

    static AssetLoader  Instance;

public:
    const errno_t   GetError() const;

    bool            OpenAsset(const char* const path);
    bool            CloseAsset();

    static AssetLoader&    GetInstance();
};