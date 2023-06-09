#pragma once
#include "AssetInterface.h"

class ScriptAsset : public AssetInterface
{
private:
    std::vector<std::string>    Functions;
    bool                        HasMainFunction = false;

public:
    ScriptAsset();

    virtual         ~ScriptAsset();
    virtual void    ParseData(const uint8_t* data) override;
};