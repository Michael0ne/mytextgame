#pragma once

#include "Generic.h"

//  An abstract asset can only be used to load asset-specific data into asset instance.
class AssetInterface
{
    std::string         Name;
    HashType            NameHash;

public:
    virtual         ~AssetInterface() {};
    virtual void    ParseData(const uint8_t* data) = 0;

    void            SetData(const std::string& name)
    {
        Name = name;
        NameHash = xxh64::hash(name.c_str(), name.length(), 0);
    }

    template <class C>
    inline C&       CastTo()
    {
        return static_cast<C&>(*this);
    }
};