#pragma once

#include "AssetInterface.h"

class GfxAsset : public AssetInterface
{
private:

public:
    GfxAsset();

    virtual         ~GfxAsset();
    virtual void    ParseData(const uint8_t* data) override;
};