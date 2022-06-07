#include "Generic.h"

#include <fstream>

static void ParseSettingsFile(FILE* filePtr, std::unordered_map<std::string, std::string>& outSettingsMap)
{
    if (!filePtr)
    {
        std::cout << LOGGER_TAG << "Empty file specified!" << std::endl;
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
            std::cout << LOGGER_TAG << "Line " << lineNumber << " has malformed key value pair! Ignoring..." << std::endl;
            continue;
        }

        equalSignSymbol[0] = NULL;

        std::pair<std::string, std::string> keyValuePair(buffer, equalSignSymbol + 1);
        outSettingsMap.emplace(keyValuePair);
    }
}

class Settings
{
private:
    static std::ifstream    File;
    static std::unordered_map<std::string, std::string> SettingsMap;

    static bool ReadSuccessfull;
    static uint32_t LinesRead;

    static Settings Instance;

    Settings() = default;

    Settings(const std::string& fileName)
    {
        ReadSuccessfull = false;

        if (fileName.empty())
        {
            std::cout << LOGGER_TAG << "Empty filename!" << std::endl;
            return;
        }

        File = std::ifstream(fileName);
        if (!File.is_open())
        {
            std::cout << LOGGER_TAG << "Failed to open settings file!" << std::endl;
            return;
        }

        LinesRead = 0;
        while (!File.eof())
        {
            std::string buffer;
            std::getline(File, buffer);

            if (buffer[0] == '#' || buffer[0] == '\0' || buffer[0] == '\r' || buffer[0] == '\n')
                continue;

            const size_t eqSignOffset = buffer.find_first_of('=');
            std::string keyStringView(buffer.begin(), buffer.begin() + eqSignOffset);
            std::string valueStringView(buffer.begin() + eqSignOffset + 1, buffer.end());

            SettingsMap.insert({ keyStringView, valueStringView });
            LinesRead++;
        }

        ReadSuccessfull = true;
    }

public:
    static void Open(const std::string& fileName)
    {
        Instance = Settings(fileName);

        std::cout << LOGGER_TAG << "Read " << LinesRead << " settings values" << std::endl;
    }

    static bool IsOpen()
    {
        return ReadSuccessfull;
    }
};

Settings Settings::Instance;
std::ifstream Settings::File;
std::unordered_map<std::string, std::string> Settings::SettingsMap;
bool Settings::ReadSuccessfull;
uint32_t Settings::LinesRead;