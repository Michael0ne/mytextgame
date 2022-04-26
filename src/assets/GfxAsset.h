#include "AssetInterface.h"

class GfxAsset : public AssetInterface
{
public:
    GfxAsset(const uint8_t* data);

    virtual         ~GfxAsset();
    virtual void    ParseData(const uint8_t* data) override;
};