#include "Loader.h"
#include "AssetInterface.h"
#include "AssetInterfaceFactory.h"

#include <iostream>
#include <fstream>

AssetLoader AssetLoader::Instance;
std::vector<AssetInterface*>    AssetLoader::Assets;

AssetLoader::AssetLoader()
{
    FileOpenStatus = -1;
    FilePtr = nullptr;
    FileDataBuffer = nullptr;
    FileSize = -1;
    AssetInterfaceRef = nullptr;
}

AssetLoader::~AssetLoader()
{
    CloseAsset();
}

bool AssetLoader::CloseAsset()
{
    if (!FilePtr)
        return false;

    fclose(FilePtr);

    FilePtr = nullptr;
    FileOpenStatus = -1;
    delete[] FileDataBuffer;
    FileSize = -1;

    return true;
}

bool AssetLoader::ParseDataFile(const std::string dataFilePath)
{
    AssetLoader& instance = GetInstance();

    //  Try and open data file that contains files to be loaded.
    //  It may also contain included files.
    std::ifstream dataFileStream(dataFilePath.c_str(), std::ios::in);
    if (!dataFileStream.is_open())
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "Can't open '{}'!", dataFilePath);
        return false;
    }

    Logger::TRACE(TAG_FUNCTION_NAME, "Reading DATA \"{}\"...", dataFilePath);

    //  Assuming file is open and good, read and instantiate all referenced assets.
    uint32_t filesRead = 0; //  How many files we already read.
    uint32_t linesRead = 0; //  How many lines (non-comments, only data lines) we read.
    std::string buffer;
    while (std::getline(dataFileStream, buffer, '\n'))
    {
        //  Skip comments.
        if (buffer[0] == '/' && buffer[1] == '/')
            continue;

        linesRead++;

        //  Skip empty line.
        //  This is done after 'lines read' is incremented, to correctly report errors placement.
        if (!buffer.length())
            continue;

        //  If it's directive.
        if (buffer[0] == '#')
        {
            //  It's an include. Open and try to parse included file.
            if (!strncmp(buffer.c_str() + 1, "include", 7))
            {
                Logger::TRACE(TAG_FUNCTION_NAME, "Parsing include \"{}\"...", (buffer.c_str() + 9));
                ParseDataFile(buffer.c_str() + 9);
                continue;
            }

            //  It's a date when this file was modified.
            if (!strncmp(buffer.c_str() + 1, "date", 4))
            {
                uint32_t day, month, year;
                day = atoi(buffer.substr(6, 2).c_str());
                month = atoi(buffer.substr(8, 2).c_str());
                year = atoi(buffer.substr(10, 4).c_str());

                Logger::TRACE(TAG_FUNCTION_NAME, "File modified on {}.{}.{}.", day, month, year);
                continue;
            }
        }

        //  Information for the engine.
        if (buffer.starts_with("info:"))
        {
            auto infoTokenType = buffer.substr(buffer.find_first_of(':') + 1);
            const auto infoTokenValue = infoTokenType.substr(infoTokenType.find_first_of(' ') + 1);
            infoTokenType = infoTokenType.substr(0, infoTokenType.find_first_of(' '));

            //  Figure out what 'info' token that was and set the engine's hint.
            if (infoTokenType == "activescene")
            {
                //  Set engine's active scene.
                SceneAsset::ActiveScene = infoTokenValue;
                Logger::TRACE(TAG_FUNCTION_NAME, "Set \"Active Scene\" to \"{}\".", SceneAsset::ActiveScene);
                continue;
            }
        }

        if (!instance.OpenAsset(buffer))
            continue;

        //  NOTE: this function is referenced in "Scene" asset loader.
        //  TODO: modify this to account for asset reference duplication and don't load it more than once.
        //  Process asset data and add it to the list.
        AssetInterface* asset = AssetInterfaceFactory::Create(instance.GetAssetType());
        instance.SetAssetRef(asset);
        asset->ParseData(instance.GetDataBufferPtr());

        //  Don't add this script asset if there was an error when parsing script.
        if (instance.GetAssetType() == eAssetType::SCRIPT && asset->CastTo<ScriptAsset>().GetErrorsFound())
        {
            instance.CloseAsset();
            filesRead++;
            continue;
        }

        Assets.push_back(asset);

        if (instance.GetAssetType() == eAssetType::SCENE)
            SceneAsset::ScenesList.push_back((SceneAsset*)asset);

        instance.CloseAsset();

        filesRead++;
    }

    Logger::TRACE(TAG_FUNCTION_NAME, "Reading DATA done. Read {} lines, found {} file references.", linesRead, filesRead);

    return true;
}

const FileErrorType AssetLoader::GetError() const
{
    return FileOpenStatus;
}

const eAssetType AssetLoader::GetAssetType() const
{
    return AssetType;
}

void AssetLoader::SetAssetRef(AssetInterface* assetInterface)
{
    AssetInterfaceRef = assetInterface;
    assetInterface->SetData(FilePath);
    assetInterface->SetDataSize(FileSize);
}

bool AssetLoader::OpenAsset(const std::string& path)
{
    if (path.empty())
        return false;

    //  Given path is relative, make it absolute.
    FilePtr = nullptr;

    //  The asset path is relative to the specified asset type.
    //  Check if it is so first.
    const size_t assetTypeStrColonPos = path.find_first_of(':');
    const size_t assetLastSlashPos = path.find_last_of('/');
    if (assetTypeStrColonPos == std::string::npos)
        return false;

    //  Set file name.
    FileName = path.substr((assetLastSlashPos == std::string::npos ? assetTypeStrColonPos : assetLastSlashPos) + 1);

    //  Record what asset type has been requested. Extension is not important.
    std::string assetType = path.substr(0, assetTypeStrColonPos);
    AssetTypeHash = xxh64::hash(assetType.c_str(), assetType.length(), 0);
    AssetType = (eAssetType)AssetTypeHash;

    //  Make full path for fopen.
    try
    {
        FilePath = AssetBaseDir;
        FilePath += AssetPathPrefix.at(AssetType);
        FilePath += path.substr(assetTypeStrColonPos + 1);
    }
    catch (std::out_of_range& exception)
    {
        UNREFERENCED_PARAMETER(exception);
        Logger::ERROR(TAG_FUNCTION_NAME, "Error: unknown asset type \"{}\".", assetType);
        return false;
    }

    FileOpenStatus = fopen_s(&FilePtr, FilePath.c_str(), "r");
    if (FileOpenStatus)
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "Can't open \"{}\".", FilePath);
        return false;
    }

    //  Allocate buffer large enough.
    fseek(FilePtr, 0l, SEEK_END);
    FileSize = ftell(FilePtr);
    rewind(FilePtr);

    FileDataBuffer = new uint8_t[FileSize];
    memset(FileDataBuffer, 0, FileSize);
    fread(FileDataBuffer, FileSize, 1, FilePtr);

    Logger::TRACE(TAG_FUNCTION_NAME, "File: {} ({} bytes) -- OK", FileName, FileSize);

    return true;
}