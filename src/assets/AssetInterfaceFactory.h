#include "Generic.h"
#include "Loader.h"
#include "TextAsset.h"
#include "GfxAsset.h"
#include "SoundAsset.h"
#include "SceneAsset.h"
#include "ScriptAsset.h"
#include "ModelAsset.h"

//  This handles AssetInterface creation with appropriate class selected.
class AssetInterfaceFactory
{
public:
    //  Based on AssetType return new instance of this asset.
    static AssetInterface* Create(const eAssetType T)
    {
        switch (T)
        {
        case eAssetType::TEXT:
            return new TextAsset;
        case eAssetType::GFX:
            return new GfxAsset;
        case eAssetType::SOUND:
            return new SoundAsset;
        case eAssetType::SCENE:
            return new SceneAsset;
        case eAssetType::SCRIPT:
            return new ScriptAsset;
        case eAssetType::MODEL:
            return new ModelAsset;
        default:
            return nullptr;
        }
    }
};