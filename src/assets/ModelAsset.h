#pragma once
#include "AssetInterface.h"

class ModelAsset : public AssetInterface
{
public:
    ModelAsset() = default;

    virtual         ~ModelAsset() override = default;
    virtual void    ParseData(const uint8_t* data) override;
};