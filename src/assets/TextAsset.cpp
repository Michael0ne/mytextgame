#include "TextAsset.h"

TextAsset::TextAsset()
{
}

TextAsset::~TextAsset()
{
}

void TextAsset::ParseData(const uint8_t* data)
{
    //  TODO: replace with std::getline.
    char* currentToken = nullptr;

    currentToken = strtok((char*)data, "\n");
    while (currentToken)
    {
        if (currentToken[0] != '#')
        {
            const size_t keyLength = strchr(currentToken, '=') - currentToken;

            const HashType keyHash = xxh64::hash(currentToken, keyLength, 0);
            const std::string value(currentToken + keyLength + 1);

            TextValues.emplace(std::make_pair(keyHash, value));
        }
        currentToken = strtok(nullptr, "\n");
    }

    std::cout << LOGGER_TAG << "Read " << TextValues.size() << " tokens" << std::endl;
}

const std::string& TextAsset::GetKeyValue(const HashType keyHash) const
{
    return TextValues.at(keyHash);
}

const std::string& TextAsset::GetKeyValue(const std::string& key) const
{
    const HashType keyHash = xxh64::hash(key.c_str(), key.length(), 0);
    return TextValues.at(keyHash);
}