#pragma once
#include "Generic.h"

//  An abstract asset can only be used to load asset-specific data into asset instance.
class AssetInterface
{
    std::string         Name;
    uint32_t            NameHash;

public:
    virtual         ~AssetInterface() {};
    virtual void    ParseData(const uint8_t* data) = 0;
};