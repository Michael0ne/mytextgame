#include "ScriptFunctions.h"
#include "SceneAsset.h"
#include "ScriptAsset.h"

std::string ScriptEngine::LastError;

//  An instance of a scripting engine expects active scene to have at least one script loaded.
//  Script execution begins within 'main' function.
//  If 'update' function is present, the it'll be called each frame.
//  At the very start of execution, the engine will 'call' main function and try to read it's control flow.
//  The function's control flow is a simple list of statements to be executed.

/// <summary>
/// Begin execution of a scripts for the current scene.
/// </summary>
/// <returns>Is start of an engine was successfull.</returns>
bool ScriptEngine::Start()
{
    //  Is there an active scene?
    if (SceneAsset::ActiveScene.empty())
    {
        std::cout << LOGGER_TAG << "Script engine requires an active scene to be present." << std::endl;
        return false;
    }

    const auto sceneRefIterator = std::find_if(SceneAsset::ScenesList.cbegin(), SceneAsset::ScenesList.cend(), [&](SceneAsset* const scene) { return scene->GetName() == SceneAsset::ActiveScene; });
    if (sceneRefIterator == SceneAsset::ScenesList.end())
    {
        std::cerr << LOGGER_TAG << "Active scene is not in loaded scenes list!" << std::endl;
        return false;
    }

    const auto scene = *sceneRefIterator;
    const auto sceneScripts = scene->GetScripts();
    if (!sceneScripts.size())
    {
        std::cout << LOGGER_TAG << "Scene '" << scene->GetName() << "' has no scripts." << std::endl;
        return true;
    }

    //  Run through all scene scripts and execute 'main' function.
    for (const auto& script : sceneScripts)
    {
        auto& thisScript = script.Asset->CastTo<ScriptAsset>();
        const auto executionResult = RunScript(thisScript);
        if (!executionResult)
        {
            std::cout << LOGGER_TAG << "Script Runtime Error: failed to run script's '" << thisScript.GetName() << "' main function in scene '" << scene->GetName() << "'." << std::endl;
            if (LastError.length())
            {
                std::cout << LOGGER_TAG << "Script Runtime Error Description: " << std::endl;
                std::cout << LOGGER_TAG << LastError << std::endl;
            }
        }
    }

    return true;
}

void ScriptEngine::Stop()
{
    std::cout << LOGGER_TAG << "Success!" << std::endl;
}

/// <summary>
/// This will run an update function for an active scene scripts.
/// </summary>
/// <param name="delta">A time delta</param>
void ScriptEngine::Update(const float_t delta)
{
    //  TODO: core logic of script execution.
}

/// <summary>
/// This will begin executing a script passed in as argument.
/// Function expects script to have full parsed script data and a 'main' function.
/// If an argument 'functionName' is not empty, then this named function will be executed.
/// If no main function is present in a script and no functionName is passed in - function will return false.
/// </summary>
/// <param name="script">A script to be executed</param>
/// <param name="functionName">Optional function name that script has, to be executed, instead of main function</param>
bool ScriptEngine::RunScript(ScriptAsset& script, const std::string& functionName)
{
    const auto functionDataRef = std::find_if(script.Functions.begin(), script.Functions.end(), [&](const ScriptAsset::FunctionDefinition& func) { return func.Name == functionName; });
    if (functionDataRef == script.Functions.end())
    {
        LastError = "Function '";
        LastError += functionName;
        LastError += "' was not found.";

        return false;
    }

    std::cout << LOGGER_TAG << "[DEBUG] Script '" << script.GetName() << "' control flow:" << std::endl;
    constexpr char ControlFlowElementTypeString[][21] = {
        "VARIABLE_ASSIGNMENT",
        "FUNCTION_CALL",
        "CONDITION_START",
        "CONDITION_BODY",
        "CONDITION_END",
    };

    size_t itemIndex = 0;
    for (const auto& cFlow :functionDataRef->ControlFlow)
    {
        std::cout << LOGGER_TAG << "[DEBUG] #" << itemIndex << ", Type: " << ControlFlowElementTypeString[cFlow.Type - 1] << ", Index: " << (int32_t)cFlow.ElementIndex << std::endl;
        itemIndex++;
    }

    return false;
}