#include "TextAsset.h"

TextAsset::TextAsset(const uint8_t* data)
{
    std::cout << "TextAsset::TextAsset" << std::endl;
}

TextAsset::~TextAsset()
{
    std::cout << "TextAsset::~TextAsset" << std::endl;
}

void TextAsset::ParseData(const uint8_t* data)
{
}

const std::string& TextAsset::GetKeyValue(const XXH64_hash_t keyHash) const
{
    return TextValues.at(keyHash);
}