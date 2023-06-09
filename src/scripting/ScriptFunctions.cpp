#include "ScriptFunctions.h"
#include "SceneAsset.h"

bool ScriptEngine::Start()
{
    if (SceneAsset::ActiveScene.empty())
    {
        std::cout << LOGGER_TAG << "No scene file specified!" << std::endl;
        return true;
    }

    for (auto scene = SceneAsset::ScenesList.begin(); scene != SceneAsset::ScenesList.end(); scene++)
    {
        for (auto script = (*scene)->GetScripts().begin(); script != (*scene)->GetScripts().end(); script++)
        {
            std::cout << LOGGER_TAG << "Script \"" << script->Name << "\" starting..." << std::endl;
        }
    }

    return true;
}

void ScriptEngine::Stop()
{
    std::cout << LOGGER_TAG << "Success!" << std::endl;
}

void ScriptEngine::Update(const float_t delta)
{

}