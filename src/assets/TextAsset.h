#include "AssetInterface.h"

class TextAsset : public AssetInterface
{
    std::unordered_map<XXH64_hash_t, std::string>   TextValues;

public:
    TextAsset();

    virtual         ~TextAsset();
    virtual void    ParseData(const uint8_t* data) override;

    //  Get text value by hashed key.
    const std::string&  GetKeyValue(const XXH64_hash_t keyHash) const;
};