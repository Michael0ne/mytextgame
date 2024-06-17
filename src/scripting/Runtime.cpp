#include "Runtime.h"
#include "SceneAsset.h"
#include "ScriptAsset.h"
#include "Logger.h"

namespace Scripting
{

    std::string Runtime::LastError;

    //  An instance of a scripting engine expects active scene to have at least one script loaded.
    //  Script execution begins within 'main' function.
    //  If 'update' function is present, the it'll be called each frame.
    //  At the very start of execution, the engine will 'call' main function and try to read it's control flow.
    //  The function's control flow is a simple list of statements to be executed.

    /// <summary>
    /// Begin execution of a scripts for the current scene.
    /// </summary>
    /// <returns>Is start of an engine was successfull.</returns>
    bool Runtime::Start()
    {
        //  Is there an active scene?
        if (SceneAsset::ActiveScene.empty())
        {
            Logger::ERROR(TAG_FUNCTION_NAME, "Script engine requires an active scene to be present.");
            return false;
        }

        const auto sceneRefIterator = std::find_if(SceneAsset::ScenesList.cbegin(), SceneAsset::ScenesList.cend(), [&](SceneAsset* const scene) { return scene->GetName() == SceneAsset::ActiveScene; });
        if (sceneRefIterator == SceneAsset::ScenesList.end())
        {
            Logger::ERROR(TAG_FUNCTION_NAME, "Active scene is not in loaded scenes list!");
            return false;
        }

        const auto scene = *sceneRefIterator;
        const auto sceneScripts = scene->GetScripts();
        if (!sceneScripts.size())
        {
            Logger::TRACE(TAG_FUNCTION_NAME, "Scene '{}' has no scripts.", scene->GetName());
            return true;
        }

        //  Run through all scene scripts and execute 'main' function.
        for (const auto& script : sceneScripts)
        {
            auto& thisScript = script.Asset->CastTo<ScriptAsset>();
            const auto executionResult = RunScript(thisScript);
            if (!executionResult)
            {
                Logger::ERROR(TAG_FUNCTION_NAME, "Script Runtime Error: failed to run script's '{}' main function in scene '{}'.", thisScript.GetName(), scene->GetName());
                if (LastError.length())
                {
                    Logger::ERROR(TAG_FUNCTION_NAME, "Script Runtime Error Description: ");
                    Logger::ERROR(TAG_FUNCTION_NAME, "{}", LastError);
                }
            }
        }

        return true;
    }

    void Runtime::Stop()
    {
        Logger::TRACE(TAG_FUNCTION_NAME, "Runtime has stopped.");
    }

    /// <summary>
    /// This will run an update function for an active scene scripts.
    /// </summary>
    /// <param name="delta">A time delta</param>
    void Runtime::Update(const float_t delta)
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
    bool Runtime::RunScript(ScriptAsset& script, const std::string& functionName)
    {
        //  There were no functions in this script at all.
        if (!script.GetFunctions().size())
        {
            LastError = "Script '";
            LastError += script.GetName();
            LastError += "' has no functions!";

            return false;
        }

        const auto functionDataRef = std::find_if(script.GetFunctions().cbegin(), script.GetFunctions().cend(), [&](const FunctionDefinition& func) { return func.Name == functionName; });
        if (functionDataRef == script.GetFunctions().end())
        {
            LastError = "Function '";
            LastError += functionName;
            LastError += "' was not found.";

            return false;
        }

        Logger::TRACE(TAG_FUNCTION_NAME, "Script '{}' control flow: ", script.GetName());
        constexpr char ControlFlowElementTypeString[][21] = {
            "VARIABLE_ASSIGNMENT",
            "FUNCTION_CALL",
            "CONDITION_START",
            "CONDITION_BODY",
            "CONDITION_END",
        };

        size_t itemIndex = 0;
        for (const auto item : functionDataRef->ControlFlow)
        {
            Logger::TRACE(TAG_FUNCTION_NAME, "#{}, Type: {}", itemIndex, ControlFlowElementTypeString[item->Type - 1]);
            itemIndex++;
        }

        return false;
    }

}