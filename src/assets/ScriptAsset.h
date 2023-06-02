#pragma once

#include "AssetInterface.h"

class ScriptAsset : public AssetInterface
{
private:

public:
    ScriptAsset();

    virtual         ~ScriptAsset();
    virtual void    ParseData(const uint8_t* data) override;
};