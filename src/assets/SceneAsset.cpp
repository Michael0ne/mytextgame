#include "SceneAsset.h"
#include "AssetInterfaceFactory.h"
#include <fstream>

std::vector<SceneAsset*> SceneAsset::ScenesList = {};
std::string SceneAsset::ActiveScene = {};

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
    EntitiesIncluded = RootValue["entries"].size();

    if (!EntitiesIncluded)
    {
        std::cout << LOGGER_TAG << "Scene file \"" << Name << "\" doesn't have any entities!" << std::endl;
        return;
    }

    //  Go through all the entities this scene includes and load them.
    for (uint32_t index = 0; index < EntitiesIncluded; index++)
    {
        const Json::Value& currentValue = RootValue["entries"][index];
        const auto assetType = (eAssetType)currentValue["type"].asUInt64();

        AssetLoader& loader = AssetLoader::GetInstance();
        if (!loader.OpenAsset(currentValue["source"].asCString()))
            continue;

        //  Process asset data and add it to the list.
        AssetInterface* asset = AssetInterfaceFactory::Create(loader.GetAssetType());
        loader.SetAssetRef(asset);
        asset->ParseData(loader.GetDataBufferPtr());

        //  Don't add this script asset if there was an error when parsing script.
        if (assetType == eAssetType::SCRIPT && asset->CastTo<ScriptAsset>().GetErrorsFound())
        {
            loader.CloseAsset();
            continue;
        }

        AssetLoader::Assets.push_back(asset);

        //  Scripts go into their own list.
        if (assetType == eAssetType::SCRIPT)
        {
            const std::string_view sourceName = currentValue["source"].asCString();
            const size_t scriptNameOffset = sourceName.find_last_of("/") + 1;

            ScriptReferenceData tempScriptEntity = {
                currentValue["id"].asUInt64(),
                sourceName.substr(scriptNameOffset, sourceName.size() - scriptNameOffset).data(),
                sourceName.data(),
                asset
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
            currentValue["parent"].asUInt(),
            asset
        };

        Entities.push_back(tempRefEntity);

        std::cout << LOGGER_TAG << "Entity: " << tempRefEntity.Name << std::endl;
    }

    ScenesList.push_back(this);
}