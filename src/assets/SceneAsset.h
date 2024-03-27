#pragma once

#include "Loader.h"
#include "AssetInterface.h"
#include "Types.h"

//	text = 0x80a69b9688ccaf52 9270267953831259986
//	gfx = 0x28a480fa8bad468a 2928607471271233162
//	sound = 0x381b96c7a2ec1dff 4042990874671193599
//	script = 0xcf7e685ca7386f66 14951502560068398950
//	scene = 0x34ebd9f0e7011c68 3813381138190244968

struct EntityReferenceData
{
    uint64_t    Id;
    std::string Name;
    eAssetType  Type;
    vec3f       Position;
    uint32_t    Width;
    uint32_t    Height;
    uint32_t    Order;
    std::string SourceAsset;
    uint64_t    ParentId;

    AssetInterface* Asset;
};

struct ScriptReferenceData
{
    uint64_t    Id;
    std::string Name;
    std::string SourceAsset;

    AssetInterface* Asset;
};

class SceneAsset : public AssetInterface
{
private:
    Json::Value     RootValue;
    uint32_t        EntitiesIncluded;
    std::vector<EntityReferenceData>    Entities;
    std::vector<ScriptReferenceData>    Scripts;

    const Json::Value& GetSectionValue(const std::string& sectionName) const;

public:
    SceneAsset();

    virtual         ~SceneAsset();
    virtual void    ParseData(const uint8_t* data) override;

    inline const std::vector<ScriptReferenceData>&   GetScripts() const
    {
        return Scripts;
    }
    inline const std::vector<EntityReferenceData>&  GetEntities() const
    {
        return Entities;
    }

    static std::vector<SceneAsset*>     ScenesList;
    static std::string                  ActiveScene;
};