#include "ScriptAsset.h"

ScriptAsset::ScriptAsset()
{
}

ScriptAsset::~ScriptAsset()
{
}

void ScriptAsset::ParseData(const uint8_t* data)
{
    char* token = nullptr;
    token = strtok((char*)data, " ");

    while (token)
    {
        //  Function definition.
        if (strstr(token, "function"))
        {
            //  Expecting a function name.
            token = strtok(nullptr, "(");

            //  Check for malformed function name.
            if (strstr(token, ")"))
            {
                std::cout << LOGGER_TAG << "Syntax parse error: expected a function name." << std::endl;
                return;
            }

            //  Remember function name.
            Functions.emplace_back(token);

            //  Remember if function is the "main".
            if (strstr(token, "main"))
                HasMainFunction = true;
        }

        token = strtok(nullptr, " ");
    }

    std::cout << LOGGER_TAG << "Found " << Functions.size() << " functions." << std::endl;
    std::cout << LOGGER_TAG << "MAIN function was " << (HasMainFunction ? "found" : "not found") << std::endl;
}