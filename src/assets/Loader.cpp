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
        std::cout << LOGGER_TAG << "Can't open " << dataFilePath << std::endl;
        return false;
    }

    std::cout << LOGGER_TAG << "Reading DATA \"" << dataFilePath << "\"" << std::endl;

    //  Assuming file is open and good, read and instantiate all referenced assets.
    uint32_t filesRead = 0; //  How many files we read.
    uint32_t linesRead = 0; //  How many lines (non-comments, only data lines) we read.
    std::string buffer;
    while (std::getline(dataFileStream, buffer, '\n'))
    {
        //  Skip comments.
        if (buffer[0] == '/' && buffer[1] == '/')
            continue;

        linesRead++;

        //  If it's directive.
        if (buffer[0] == '#')
        {
            //  It's an include. Open and try to parse included file.
            if (!strncmp(buffer.c_str() + 1, "include", 7))
            {
                std::cout << LOGGER_TAG << "Parsing include \"" << (buffer.c_str() + 9) << "\"" << std::endl;
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

                std::cout << LOGGER_TAG << "File modified on " << day << "/" << month << "/" << year << std::endl;
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

        Assets.push_back(asset);

        if (instance.GetAssetType() == eAssetType::SCENE)
            SceneAsset::ScenesList.push_back((SceneAsset*)asset);

        instance.CloseAsset();

        filesRead++;
    }

    std::cout << LOGGER_TAG << "Reading DATA done. " << linesRead << " lines, " << filesRead << " file references." << std::endl;

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
        std::cout << LOGGER_TAG << "Error: unknown asset type \"" << assetType << "\"" << std::endl;
        return false;
    }

    FileOpenStatus = fopen_s(&FilePtr, FilePath.c_str(), "r");
    if (FileOpenStatus)
    {
        std::cout << LOGGER_TAG "Can't open \"" << FilePath << "\"!" << std::endl;
        return false;
    }

    //  Allocate buffer large enough.
    fseek(FilePtr, 0l, SEEK_END);
    FileSize = ftell(FilePtr);
    rewind(FilePtr);

    FileDataBuffer = new uint8_t[FileSize];
    memset(FileDataBuffer, 0, FileSize);
    fread(FileDataBuffer, FileSize, 1, FilePtr);

    std::cout << LOGGER_TAG << "File: " << FileName << " (" << FileSize << " bytes) -- OK" << std::endl;

    return true;
}