#include "TextAsset.h"

TextAsset::TextAsset()
{
}

TextAsset::~TextAsset()
{
}

void TextAsset::ParseData(const uint8_t* data)
{
    char* nextToken = nullptr;
    char* currentToken = nullptr;

    currentToken = strtok_s((char*)data, "\n", &nextToken);
    while (currentToken)
    {
        if (currentToken[0] != '#')
        {
            const size_t keyLength = strchr(currentToken, '=') - currentToken;

            const XXH64_hash_t keyHash = XXH64(currentToken, keyLength, NULL);
            const std::string value(currentToken + keyLength + 1);

            TextValues.emplace(std::make_pair(keyHash, value));
        }
        currentToken = strtok_s(nullptr, "\n", &nextToken);
    }
}

const std::string& TextAsset::GetKeyValue(const XXH64_hash_t keyHash) const
{
    return TextValues.at(keyHash);
}

const std::string& TextAsset::GetKeyValue(const std::string& key) const
{
    const XXH64_hash_t keyHash = XXH64(key.c_str(), key.length(), NULL);
    return TextValues.at(keyHash);
}