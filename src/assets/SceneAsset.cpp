#include "SceneAsset.h"
#include "AssetInterfaceFactory.h"
#include "Logger.h"
#include "Settings.h"
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
        Logger::ERROR(TAG_FUNCTION_NAME, "Failed to parse '{}'!", Name);
        Logger::ERROR(TAG_FUNCTION_NAME, "Errors: {}", reader.getFormattedErrorMessages());

        return;
    }

    //  Update entities included value to reflect how many entities there are in scene.
    EntitiesIncluded = RootValue["entries"].size();

    if (!EntitiesIncluded)
    {
        Logger::ERROR(TAG_FUNCTION_NAME, "Scene file \"{}\" doesn't have any entities!", Name);
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

        //  Don't load script asset if scripts are disabled.
        if (assetType == eAssetType::SCRIPT && Settings::GetValue<bool>("scripts", true) == false)
        {
            loader.CloseAsset();
            continue;
        }

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

            Logger::TRACE(TAG_FUNCTION_NAME, "Script: {}", tempScriptEntity.SourceAsset);

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

        Logger::TRACE(TAG_FUNCTION_NAME, "Entity: {}", tempRefEntity.Name);
    }

    ScenesList.push_back(this);
}