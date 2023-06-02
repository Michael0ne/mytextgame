#include "SceneAsset.h"

#include <fstream>

const Json::Value& SceneAsset::GetSectionValue(const std::string& sectionName) const
{
        return !RootValue ? RootValue : RootValue[sectionName];
}

SceneAsset::SceneAsset()
{
    EntitiesIncluded = 0;
}

SceneAsset::~SceneAsset()
{
}

void SceneAsset::ParseData(const uint8_t* data)
{
    //  Parse actual contents of the buffer into JSON object.
    Json::String errorString;
    Json::Reader reader;

    if (!reader.parse((const char*)data, (const char*)data + DataSize, RootValue))
    {
        std::cout << LOGGER_TAG << "Failed to parse " << Name << std::endl;
        std::cout << LOGGER_TAG << "Errors: " << reader.getFormattedErrorMessages() << std::endl;

        return;
    }

    //  Update entities included value to reflect how many entities there are in scene.
    EntitiesIncluded = RootValue["menu"].size();

    if (!EntitiesIncluded)
    {
        std::cout << LOGGER_TAG << "Scene file \"" << Name << "\" doesn't have any entities!" << std::endl;
        return;
    }

    //  Go through all the entities this scene includes and load them.
    for (uint32_t index = 0; index < EntitiesIncluded; index++)
    {
        const Json::Value& currentValue = RootValue["menu"][index];
        const auto assetType = (eAssetType)currentValue["type"].asUInt64();

        //  Scripts go into their own list.
        if (assetType == eAssetType::SCRIPT)
        {
            ScriptReferenceData tempScriptEntity = {
                currentValue["id"].asUInt64(),
                currentValue["source"].asCString()
            };

            Scripts.push_back(tempScriptEntity);

            std::cout << LOGGER_TAG << "Script: " << tempScriptEntity.SourceAsset << std::endl;
            continue;
        }

        //  The rest goes into the Entities list.
        EntityReferenceData tempRefEntity = {
            currentValue["id"].asUInt64(),
            currentValue["name"].asCString(),
            assetType,
            { currentValue["position"][0].asFloat(), currentValue["position"][1].asFloat(), currentValue["position"][2].asFloat() },
            currentValue["width"].asUInt(),
            currentValue["height"].asUInt(),
            currentValue["order"].asUInt(),
            currentValue["source"].asCString(),
            currentValue["parent"].asUInt()
        };

        Entities.push_back(tempRefEntity);

        std::cout << LOGGER_TAG << "Entity: " << tempRefEntity.Name << std::endl;
    }
}