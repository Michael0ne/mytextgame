#include "Generic.h"

#define SETTINGS_FILE_NAME "settings.txt"

static void ParseSettingsFile(FILE* filePtr, std::unordered_map<std::string, std::string>& outSettingsMap)
{
    if (!filePtr)
    {
        std::cout << "[Settings] ParseSettingsFile file pointer is NULL!" << std::endl;
        return;
    }

    char buffer[512] = {};
    uint32_t lineNumber = 0;
    while (fgets(buffer, sizeof(buffer), filePtr))
    {
        lineNumber++;

        if (buffer[0] == '#')
            continue;

        char* newLineSymbol = strrchr(buffer, '\n');
        if (newLineSymbol)
            newLineSymbol[0] = NULL;

        char* const equalSignSymbol = strchr(buffer, '=');
        if (!equalSignSymbol)
        {
            std::cout << "[Settings] ParseSettingsFile: line " << lineNumber << " has malformed key value pair! Ignoring..." << std::endl;
            continue;
        }

        equalSignSymbol[0] = NULL;

        std::pair<std::string, std::string> keyValuePair(buffer, equalSignSymbol + 1);
        outSettingsMap.emplace(keyValuePair);
    }
}