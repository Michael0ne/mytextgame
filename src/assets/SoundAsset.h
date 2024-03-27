#pragma once

#include "AssetInterface.h"

class SoundAsset : public AssetInterface
{
public:
    SoundAsset();

    virtual         ~SoundAsset();
    virtual void    ParseData(const uint8_t* data) override;
};