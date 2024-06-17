#pragma once
#include "AssetInterface.h"

namespace Scripting
{
    struct VariableDefinition
    {
        std::string     Name;
        std::string     Value;
    };

    //  The base for a control flow statement.
    //  Only 'type' is required, because rest is statement-specific.
    struct ControlFlowElement
    {
        enum ControlFlowElementType
        {
            VARIABLE_ASSIGNMENT = 1,
            FUNCTION_CALL,
            CONDITION_START,
            CONDITION_BODY,
            CONDITION_OPERATOR,
            CONDITION_END,
        }               Type;

        virtual ~ControlFlowElement() = default;
    };

    //  A statement that describes a variable assignment statement.
    //  Example: var_1 = 1
    //           ^^^^^^^^^
    //          that's the statement.
    //  The variable with the name 'variablename' (referenced in this context by it's 'variableindex') is assigned a computed value of right hand side.
    struct AssignmentStatement : public ControlFlowElement
    {
        std::string     VariableName;
        size_t          VariableIndex;
        std::string     RHS;

        inline AssignmentStatement(const std::string variableName, const size_t variableIndex, const std::string rightHandSide)
        {
            Type = VARIABLE_ASSIGNMENT;
            VariableName = variableName;
            VariableIndex = variableIndex;
            RHS = rightHandSide;
        }
    };

    //  A statement that describes a function call.
    //  Example: myFunction(arg1)
    //           ^^^^^^^^^^^^^^^^
    //          that's the statement.
    //  Essentially, this describes a function call:
    //  it's name, parsed arguments list and a global index to that function in Functions list.
    struct FunctionCallStatement : public ControlFlowElement
    {
        std::string     FunctionName;
        std::vector<std::string>    Arguments;
        size_t          GlobalFunctionIndex;

        inline FunctionCallStatement(const std::string functionName, const std::vector<std::string> arguments, const size_t globalFunctionIndex)
        {
            Type = FUNCTION_CALL;
            FunctionName = functionName;
            Arguments = arguments;
            GlobalFunctionIndex = globalFunctionIndex;
        }
    };

    //  A statement that describes a body of a condition.
    //  Example: if (var_1 > 1)
    //               ^^^^^^^^^
    //              that's the statement.
    //  There are Left Hand Side and Right Hand Side of the statement.
    struct ConditionStatement : public ControlFlowElement
    {
        enum tConditionType
        {
            CONDITION_TYPE_NONE = 0,
            CONDITION_TYPE_EQUALS,
            CONDITION_TYPE_NOT_EQUALS,
            CONDITION_TYPE_LESS_THAN,
            CONDITION_TYPE_GREATER_THAN,
            CONDITION_TYPE_LESSOREQUAL_THAN,
            CONDITION_TYPE_GREATEROREQUAL_THAN,
        }   ConditionType;

        std::string     LHS;
        std::string     RHS;

        //  This version of constructor is used to describe an actual body of 'if' statement.
        explicit inline ConditionStatement(const tConditionType conditionType, const std::string leftHandSide, const std::string rightHandSide)
        {
            Type = CONDITION_BODY;
            ConditionType = conditionType;

            LHS = leftHandSide;
            RHS = rightHandSide;
        }

        //  This version is used to mark a beginning and an end of a condition block.
        explicit inline ConditionStatement(const ControlFlowElementType type)
        {
            Type = type;
            ConditionType = CONDITION_TYPE_NONE;
            LHS = {};
            RHS = {};
        }
    };
    
    //  A statement that describes what 'glues' two (left and right) condition statements.
    //  Example: if (statement1) AND (statement2).
    //                           ^^^
    //                          that's the operator.
    struct ConditionRelationStatement : public ControlFlowElement
    {
        enum ConditionOperator
        {
            CONDITION_OPERATOR_LOGICAL_AND,
            CONDITION_OPERATOR_LOGICAL_OR,
        }               RelationType;
    };

    //  An actual function that script contains.
    //  This includes the function's name, arguments list, local variables and a control flow.
    struct FunctionDefinition
    {
        std::string                     Name;
        std::vector<std::string>        Arguments;
        std::vector<VariableDefinition> Variables;
        std::vector<ControlFlowElement*> ControlFlow;

        //  FIXME: this will sometims cause a crash! Probably, something is left uninitialized...
        ~FunctionDefinition()
        {
            if (ControlFlow.size())
            {
                for (size_t i = 0; i < ControlFlow.size(); i++)
                {
                    if (ControlFlow[i])
                        delete ControlFlow[i];
                }
            }
        }

        FunctionDefinition()
        {
            Name = {};
            Arguments = {};
            Variables = {};
            ControlFlow = {};
        }
    };
};

class ScriptAsset : public AssetInterface
{
protected:
    std::vector<Scripting::FunctionDefinition>     Functions;
    uint32_t                            ErrorsFound;

public:
    ScriptAsset();

    virtual         ~ScriptAsset();
    virtual void    ParseData(const uint8_t* data) override;

    inline const uint32_t  GetErrorsFound() const
    {
        return ErrorsFound;
    }

    inline const auto GetFunctions() const
    {
        return Functions;
    }
};