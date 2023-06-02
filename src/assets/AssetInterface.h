#pragma once

#include "Generic.h"

//  An abstract asset can only be used to load asset-specific data into asset instance.
class AssetInterface
{
protected:
    std::string         Name;
    HashType            NameHash;
    size_t              DataSize;

public:
    virtual         ~AssetInterface() {};
    virtual void    ParseData(const uint8_t* data) = 0;

    void            SetData(const std::string& name)
    {
        Name = name;
        NameHash = xxh64::hash(name.c_str(), name.length(), 0);
    }

    inline void     SetDataSize(const size_t size)
    {
        DataSize = size;
    }

    template <class C>
    inline C&       CastTo()
    {
        return static_cast<C&>(*this);
    }
};