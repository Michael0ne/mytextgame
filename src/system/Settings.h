#include "Generic.h"
#include "Logger.h"

#include <fstream>

class Settings
{
private:
    std::ifstream    File;
    std::string      FileName;
    std::unordered_map<std::string, std::string> SettingsMap;

    bool ReadSuccessfull;
    bool HasChanges;
    uint32_t LinesRead;

    static std::unordered_map<HashType, std::string> PredefinedTokens;

    void ParseTokenValue(std::string& val)
    {
        //  Possibly, insert additional 'search' functions here to find token value anywhere in the engine.
        const HashType tokenHash = xxh64::hash(val.c_str() + 1, val.length() - 1, 0);
        try
        {
            val = PredefinedTokens.at(tokenHash);
        }
        catch (std::out_of_range e)
        {
            Logger::ERROR(TAG_FUNCTION_NAME, "Token \"{}\" ({}) has no predefined value!", val.c_str() + 1, tokenHash);
        }
    }

    void MakeDefaultSettingsFile(const std::string& fileName)
    {
        Logger::TRACE(TAG_FUNCTION_NAME, "Making a default one...");

        std::ofstream outFile(fileName);
        outFile << "# generated";

        outFile.close();
    }

    Settings() = default;
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings(Settings&) = delete;
    Settings& operator=(Settings&&) = delete;

    Settings(const std::string& fileName)
    {
        ReadSuccessfull = false;
        HasChanges = false;
        LinesRead = 0;

        if (fileName.empty())
        {
            Logger::ERROR(TAG_FUNCTION_NAME, "Empty filename!");
            return;
        }

        File = std::ifstream(fileName);
        if (!File.is_open())
        {
            Logger::ERROR(TAG_FUNCTION_NAME, "Settings file not found!");
            MakeDefaultSettingsFile(fileName);

            File = std::ifstream(fileName);
        }

        FileName = fileName;
        while (!File.eof())
        {
            std::string buffer;
            std::getline(File, buffer);

            //  Is this a commentary?
            if (buffer[0] == '#' || buffer[0] == '\0' || buffer[0] == '\r' || buffer[0] == '\n')
                continue;

            //  Split string into 'key = value' pair.
            const size_t eqSignOffset = buffer.find_first_of('=');
            const std::string keyString(buffer.begin(), buffer.begin() + eqSignOffset);
            std::string valueString(buffer.begin() + eqSignOffset + 1, buffer.end());

            //  Are there pre-defined tokens in the string?
            size_t refConstValue = valueString.find_first_of('$');
            while (refConstValue != std::string::npos)
            {
                //  Go through all of pre-defined tokens in this string and replace with it's value.
                size_t nextSpaceOffset = valueString.find_first_of(' ', refConstValue);
                if (nextSpaceOffset == std::string::npos)
                    nextSpaceOffset = valueString.length();

                std::string tokenValue = valueString.substr(refConstValue, nextSpaceOffset - refConstValue);

                ParseTokenValue(tokenValue);
                valueString.replace(refConstValue, nextSpaceOffset - refConstValue, tokenValue);

                refConstValue = valueString.find_first_of('$', refConstValue + 1);
            }

            SettingsMap.insert({ keyString, valueString });
            LinesRead++;
        }

        File.close();

        ReadSuccessfull = true;
    }

    ~Settings()
    {
        if (!HasChanges || FileName.empty())
            return;

        std::ofstream outFile(FileName);
        outFile << "# updated " << time(nullptr) << std::endl;

        for (auto settingsOption = SettingsMap.cbegin(); settingsOption != SettingsMap.cend(); settingsOption++)
            outFile << settingsOption->first << "=" << settingsOption->second << std::endl;

        outFile.close();

        Logger::TRACE(TAG_FUNCTION_NAME, "Changes saved!");
    }

    static Settings*    Instance;

public:
    static void Open(const std::string& fileName)
    {
        Instance = new Settings(fileName);

        Logger::TRACE(TAG_FUNCTION_NAME, "Read {} settings values.", Instance->LinesRead);
    }

    static void Shutdown()
    {
        delete Instance;
    }

    static bool IsOpen()
    {
        return Instance->ReadSuccessfull;
    }

    static const std::string& GetValue(const std::string& key, const std::string& defValue)
    {
        try
        {
            return Instance->SettingsMap.at(key);
        }
        catch (std::out_of_range)
        {
            return defValue;
        }
    }

    static bool RemoveValue(const std::string& key)
    {
        try
        {
            Instance->SettingsMap.erase(key);
            return true;
        }
        catch (std::out_of_range)
        {
            return false;
        }
    }

    static void WriteValue(const std::string& key, const std::string& value)
    {
        Instance->HasChanges = true;
        Instance->SettingsMap[key] = value;
    }
};

Settings* Settings::Instance = nullptr;
std::unordered_map<HashType, std::string> Settings::PredefinedTokens =
{
    { 0x0840acdf6fe13b74, "1920" },
    { 0xd93c80b41d41202f, "1080" }
};