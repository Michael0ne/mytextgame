#pragma once

#include "Generic.h"

//  An abstract asset can only be used to load asset-specific data into asset instance.
class AssetInterface
{
    std::string         Name;
    XXH64_hash_t        NameHash;

public:
    virtual         ~AssetInterface() {};
    virtual void    ParseData(const uint8_t* data) = 0;

    void            SetData(const std::string& name)
    {
        Name = name;
        NameHash = XXH64(name.c_str(), name.length(), NULL);
    }

    template <class C>
    inline C&       CastTo()
    {
        return *(C*)this;
    }
};