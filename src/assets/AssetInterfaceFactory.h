#include "Generic.h"
#include "Loader.h"
#include "TextAsset.h"
#include "GfxAsset.h"
#include "SoundAsset.h"

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
            break;
        case eAssetType::GFX:
            return new GfxAsset;
            break;
        case eAssetType::SOUND:
            return new SoundAsset;
            break;
        default:
            return nullptr;
            break;
        }
    }
};