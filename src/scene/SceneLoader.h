#include "Generic.h"

class SceneLoader
{
private:
    Json::Value     RootValue;

    bool            HasOpen;

public:
    SceneLoader(const std::string& fileName);

    void PrintSectionParameters(const std::string& sectionName) const;
    inline const bool IsOpen() const
    {
        return HasOpen;
    }
};