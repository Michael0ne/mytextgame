#include "AssetInterface.h"

class SoundAsset : public AssetInterface
{
public:
    SoundAsset(const uint8_t* data);

    virtual         ~SoundAsset();
    virtual void    ParseData(const uint8_t* data) override;
};