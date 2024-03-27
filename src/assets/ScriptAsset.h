#pragma once
#include "AssetInterface.h"
#include <deque>

class ScriptAsset : public AssetInterface
{
    friend class ScriptEngine;
protected:
    struct VariableDefinition
    {
        std::string     Name;
        std::string     Value;
    };

    struct ControlFlowElement
    {
        enum
        {
            VARIABLE_ASSIGNMENT = 1,
            FUNCTION_CALL,
            CONDITION_START,
            CONDITION_BODY,
            CONDITION_END,
        }               Type;

        size_t          ElementIndex;
    };

    struct FunctionDefinition
    {
        std::string                     Name;
        std::vector<std::string>        Arguments;
        std::vector<VariableDefinition> Variables;
        std::deque<ControlFlowElement>  ControlFlow;
    };

    std::vector<FunctionDefinition>     Functions;
    uint32_t                            ErrorsFound;

public:
    ScriptAsset();

    virtual         ~ScriptAsset();
    virtual void    ParseData(const uint8_t* data) override;

    inline const uint32_t  GetErrorsFound() const
    {
        return ErrorsFound;
    }
};