#include "Generic.h"

class SceneLoader
{
private:
    Json::Value     RootValue;

    bool            HasOpen;

public:
    SceneLoader(const std::string& fileName);

    const Json::Value& GetSectionValue(const std::string& sectionName) const;
    inline const bool IsOpen() const
    {
        return HasOpen;
    }
};