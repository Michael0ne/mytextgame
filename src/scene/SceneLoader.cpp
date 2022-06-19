#include "SceneLoader.h"

#include <fstream>

SceneLoader::SceneLoader(const std::string& fileName)
{
    HasOpen = false;

    if (fileName.empty())
    {
        std::cout << LOGGER_TAG << "Empty scene file specified!" << std::endl;
        return;
    }

    Json::String errs;
    Json::CharReaderBuilder builder;

    std::ifstream ifstream(fileName.c_str());
    if (!ifstream.is_open())
    {
        std::cout << LOGGER_TAG << "Can't open scene file \"" << fileName << "\"!" << std::endl;
        return;
    }

    if (!Json::parseFromStream(builder, ifstream, &RootValue, &errs))
    {
        std::cout << LOGGER_TAG << "Can't parse scene file! " << std::endl;
        std::cout << errs << std::endl;
        return;
    }

    HasOpen = true;
}

const Json::Value& SceneLoader::GetSectionValue(const std::string& sectionName) const
{
    if (!IsOpen() || !RootValue)
        return RootValue;

    return RootValue[sectionName];
}