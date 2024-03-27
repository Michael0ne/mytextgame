#pragma once

#include "AssetInterface.h"

class TextAsset : public AssetInterface
{
    std::unordered_map<HashType, std::string>   TextValues;

public:
    TextAsset();

    virtual         ~TextAsset();
    virtual void    ParseData(const uint8_t* data) override;

    //  Get text value by hashed key.
    const std::string&  GetKeyValue(const HashType keyHash) const;
    //  Get text value by key string.
    const std::string&  GetKeyValue(const std::string& key) const;
};