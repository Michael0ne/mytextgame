#include "ScriptAsset.h"
#include "Logger.h"

ScriptAsset::ScriptAsset()
{
    ErrorsFound = 0;
}

ScriptAsset::~ScriptAsset()
{
}

//  TODO:   this implementation is very trivial - LOTS of redundant string copies on each iteration. Once all parser stuff is done - re-do without using string copy.
/// <summary>
/// Parse input <param>data</param> string and generate an IR code to be executed later by the scripting engine.
/// Important! This method expects to not be called manually, since it depends on a 'DataSize' value of this class instance to be set before.
/// </summary>
/// <param name="data">A pointer to a buffer containing complete script strings</param>
void ScriptAsset::ParseData(const uint8_t* data)
{
    //  NOTE: half of these are not needed with current implementation, that's for future.
    struct tParserState
    {
        bool            IsFunction              = false;
        bool            IsFunctionArguments     = false;
        bool            IsFunctionBody          = false;
        bool            IsCommentary            = false;
        bool            IsAssignmentOperator    = false;
        bool            IsFunctionCall          = false;
        bool            IsFunctionCallArguments = false;
        bool            IsString                = false;
        bool            IsVariableDeclaration   = false;
        bool            IsIncludeDirective      = false;
        bool            IsCondition             = false;

        struct tContext
        {
            size_t      LastFunctionIndex = 0;
            std::string FunctionName = {};
        }               Context = {};
    } ParserState;

    //  This will modify the 'subject' string to replace all occurrences of 'search' with 'replace'.
    const auto ReplaceStringInPlace = [](std::string& subject, const std::string& search, const std::string& replace)
        {
            size_t pos = 0;
            while ((pos = subject.find(search, pos)) != std::string::npos) {
                subject.replace(pos, search.length(), replace);
                pos += replace.length();
            }
        };

    //  This will remove all 'search' characters from the beginning of a string.
    const auto TrimLeftInPlace = [](std::string& subject, const char search)
        {
            if (subject.starts_with(search) && subject.length())
                subject = subject.substr(subject.find_first_not_of(search));
        };

    //  This version is to be used in a function definition string.
    //  Parsed output will be written to an 'arguments' list.
    const auto ParseArgumentsList = [&](const std::string& input, const std::string& functionName, std::vector<std::string>& arguments)
        {
            if (!input.length())
                return;

            std::stringstream argumentsString(input);
            std::string token;
            size_t argumentIndex = 1;

            while (std::getline(argumentsString, token, ','))
            {
                if (token.length())
                    ReplaceStringInPlace(token, " ", "");

                if (!token.length())
                {
                    Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: empty argument #{} name in '{}' function arguments list.", argumentIndex, functionName);
                    ErrorsFound++;
                    argumentIndex++;
                    continue;
                }

                if (token == "function")
                {
                    Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: unexpected usage of keyword as #{} '{}' function argument", argumentIndex, functionName);
                    ErrorsFound++;
                    argumentIndex++;
                    continue;
                }

                arguments.push_back(token);
                argumentIndex++;
            }
        };

    //  This version is to be used in a function call string.
    //  Parsed arguments list will be written into 'arguments' list.
    //  It's different because of support for string arguments (quoted string). Since function definition only describes arguments names - function call, however, expects actual arguments values and that can include string.
    const auto ParseArgumentsListCall = [&](const std::string& input, const std::string& functionName, const std::string& calleeFunctionName, std::vector<std::string>& arguments)
        {
            if (!input.length())
                return;

            std::stringstream argumentsString(input);
            std::string token;
            size_t argumentIndex = 1;
            bool isQuotedString = false;
            std::string quotedString;

            while (std::getline(argumentsString, token, ','))
            {
                if (token.length() && !isQuotedString)
                    ReplaceStringInPlace(token, " ", "");

                if (!token.length())
                {
                    Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: empty function '{}' call argument #{}, called from {}.", functionName, argumentIndex, calleeFunctionName);
                    ErrorsFound++;
                    argumentIndex++;
                    continue;
                }

                const auto quoteCharPos = token.find_first_of('"');
                if (isQuotedString)
                {
                    //  Is it end of a quoted string?
                    if (quoteCharPos != std::string::npos)
                    {
                        quotedString += token.substr(0, quoteCharPos);

                        arguments.push_back(quotedString);

                        quotedString.clear();
                        isQuotedString = false;
                        argumentIndex++;
                        continue;
                    }

                    //  No quote character in this string and it's a quoted string - add it to the last buffer and go on.
                    quotedString += token;
                    quotedString += ",";
                    continue;
                }
                else
                {
                    //  Not a quoted string, just an argument name.
                    if (quoteCharPos == std::string::npos)
                    {
                        arguments.push_back(token);

                        argumentIndex++;
                        continue;
                    }

                    //  Beginning of a quoted string?
                    if (quoteCharPos == 0)
                    {
                        const auto lastQuotePos = token.find_last_of('"');

                        //  Is there immediately an end quote in the string?
                        if (lastQuotePos != std::string::npos && lastQuotePos != 0)
                        {
                            arguments.push_back(token.substr(1, lastQuotePos - 1));
                            argumentIndex++;
                            continue;
                        }

                        //  There's no quote character in this string after start quote.
                        quotedString = token.substr(1);
                        quotedString += ",";
                        isQuotedString = true;
                        continue;
                    }

                    //  That's an error if none of the above worked out.
                    Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: unexpected token found in '{}' function call, called from '{}'.", functionName, calleeFunctionName);
                    ErrorsFound++;
                }
            }
        };

    //  Make a copy of the string to remove all unnecessary stuff.
    std::string dataString((const char* const)data, DataSize);
    ReplaceStringInPlace(dataString, "\t", "");
    ReplaceStringInPlace(dataString, "  ", "");

    //  String stream to iterate through and tokenize each line.
    std::stringstream parserDataString(dataString);
    std::string currentTokenString;

    while (std::getline(parserDataString, currentTokenString, '\n'))
    {
        Logger::TRACE(TAG_FUNCTION_NAME, "Token: {}", currentTokenString);

        if (!currentTokenString.length())
            continue;

        TrimLeftInPlace(currentTokenString, ' ');

        //  Is it commentary?
        if (currentTokenString.starts_with("//"))
        {
            continue;
        }

        //  Is it include directive?
        if (currentTokenString.starts_with("#include"))
        {
            //  Can't have that inside function body.
            if (ParserState.IsFunctionBody)
            {
                Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: unexpected include keyword found inside function body.");
                ErrorsFound++;
                return;
            }

            auto quotedString = currentTokenString.substr(currentTokenString.find_first_of('"') + 1);
            quotedString = quotedString.substr(0, quotedString.find_first_of('"') - 1);

            if (!quotedString.length())
            {
                Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: empty include directive found.");
                ErrorsFound++;
                return;
            }

            //  TODO: parse include directive. That will make an instance of 'ScriptAsset' and pass this directive path there.
            continue;
        }

        //  Is it function?
        if (currentTokenString.starts_with("function"))
        {
            //  Can't have another function definition inside function body.
            if (ParserState.IsFunctionBody)
            {
                Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: unexpected function keyword found inside function body.");
                ErrorsFound++;
                return;
            }

            //  Malformed function definition?
            if (currentTokenString.find_first_of('(') == std::string::npos || currentTokenString.find_first_of(')') == std::string::npos)
            {
                Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: malformed function definition! Can't find arguments list.");
                ErrorsFound++;
                return;
            }

            ParserState.IsFunction = true;

            //  Get function name.
            auto functionName = currentTokenString.substr(currentTokenString.find_first_of(' '));
            functionName = functionName.substr(1, functionName.find_first_of('(') - 1);

            //  Get function arguments.
            auto argumentsString = currentTokenString.substr(currentTokenString.find_first_of('('));
            argumentsString = argumentsString.substr(1, argumentsString.find_first_of(')') - 1);

            Scripting::FunctionDefinition thisFunction;
            thisFunction.Name = functionName;

            ParseArgumentsList(argumentsString, functionName, thisFunction.Arguments);

            Functions.push_back(thisFunction);

            ParserState.Context.FunctionName = functionName;
            ParserState.Context.LastFunctionIndex = Functions.size() - 1;

            continue;
        }

        //  Function body?
        if (currentTokenString.starts_with("{"))
        {
            ParserState.IsFunctionBody = true;
            continue;
        }

        //  Function body ends?
        if (ParserState.IsFunctionBody && currentTokenString.starts_with("}"))
        {
            ParserState.IsFunctionBody = false;
            ParserState.IsFunction = false;

            continue;
        }

        //  Is it function call?
        if (!currentTokenString.starts_with("if") && currentTokenString.find_first_of("=") == std::string::npos && currentTokenString.find_first_of("(") != std::string::npos && currentTokenString.find_first_of(")") != std::string::npos)
        {
            ParserState.IsFunctionCall = true;

            auto calledFunctionName = currentTokenString.substr(0, currentTokenString.find_first_of('('));
            ReplaceStringInPlace(calledFunctionName, " ", "");

            auto calledFunctionArguments = currentTokenString.substr(currentTokenString.find_first_of('(') + 1);
            calledFunctionArguments = calledFunctionArguments.substr(0, calledFunctionArguments.find_first_of(')'));

            //  Check if that function exists.
            //  Actually, don't do that. The function might be declared afterwards, so let's do it after all parser stuff is done.
            /*
            const auto functionRef = std::find_if(Functions.begin(), Functions.end(), [&](const Scripting::FunctionDefinition& f) { return f.Name == calledFunctionName; });
            if (functionRef == std::end(Functions))
            {
                Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: function '{}' called from '{}' was not found.", calledFunctionName, ParserState.Context.FunctionName);
                ErrorsFound++;
                return;
            }
            */

            //  Parse arguments list.
            std::vector<std::string> argumentsList;
            ParseArgumentsListCall(calledFunctionArguments, calledFunctionName, ParserState.Context.FunctionName, argumentsList);
            /*
            //  Check if number of arguments that's passed in called function match the expected number of arguments for that function.
            if (argumentsList.size() != functionRef->Arguments.size())
            {
                Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: function's '{}' number of arguments doesn't match the function prototype, called from '{}'.", calledFunctionName, ParserState.Context.FunctionName);
                ErrorsFound++;
                return;
            }
            */

            auto currentFunction = std::find_if(Functions.begin(), Functions.end(), [&](const Scripting::FunctionDefinition& f) { return ParserState.Context.FunctionName == f.Name; });

            auto item = new Scripting::FunctionCallStatement(calledFunctionName, argumentsList, 0);
            currentFunction->ControlFlow.push_back(item);

            ParserState.IsFunctionCall = false;

            continue;
        }

        //  Is it condition statement?
        if (currentTokenString.starts_with("if") && currentTokenString.find_first_of("(") != std::string::npos && currentTokenString.find_first_of(")") != std::string::npos)
        {
            ParserState.IsCondition = true;

            auto conditionStatement = currentTokenString.substr(currentTokenString.find_last_of("(") + 1);
            conditionStatement = conditionStatement.substr(0, conditionStatement.find_first_of(")") - 1);

            //  No condition statement?
            if (!conditionStatement.length())
            {
                Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: function's '{}' condition statement is missing it's body.", ParserState.Context.FunctionName);
                ErrorsFound++;
                return;
            }

            auto currentFunction = std::find_if(Functions.begin(), Functions.end(), [&](const Scripting::FunctionDefinition& f) { return ParserState.Context.FunctionName == f.Name; });

            //currentFunction->ControlFlow.push_back({ Scripting::ControlFlowElement::CONDITION_START, (size_t)-1 });

            continue;
        }

        //  Condition statement ends?
        if (currentTokenString.starts_with("endif") && ParserState.IsCondition && ParserState.IsFunctionBody)
        {
            ParserState.IsCondition = false;

            auto currentFunction = std::find_if(Functions.begin(), Functions.end(), [&](const Scripting::FunctionDefinition& f) { return ParserState.Context.FunctionName == f.Name; });

            auto item = new Scripting::ConditionStatement(Scripting::ControlFlowElement::CONDITION_END);
            currentFunction->ControlFlow.push_back(item);

            continue;
        }

        //  Assignment operator?
        if (currentTokenString.find_first_of("=") != std::string::npos)
        {
            const auto equalsOpPos = currentTokenString.find_first_of('=');

            auto varName = currentTokenString.substr(0, equalsOpPos);
            ReplaceStringInPlace(varName, " ", "");

            auto varRightSide = currentTokenString.substr(equalsOpPos + 1);
            ReplaceStringInPlace(varRightSide, " ", "");

            auto currentFunction = std::find_if(Functions.begin(), Functions.end(), [&](const Scripting::FunctionDefinition& f) { return ParserState.Context.FunctionName == f.Name; });

            if (!varName.length() || !varRightSide.length())
            {
                Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: malformed '{}' function's variable syntax.", currentFunction->Name);
                ErrorsFound++;
                return;
            }

            currentFunction->Variables.push_back({ varName, varRightSide });

            auto item = new Scripting::AssignmentStatement(varName, currentFunction->Variables.size() - 1, varRightSide);
            currentFunction->ControlFlow.push_back(item);

            continue;
        }

        //  None of the above. Unknown string.
        Logger::ERROR(TAG_FUNCTION_NAME, "Syntax Parse Error: unknown token found '{}'.", currentTokenString);
        ErrorsFound++;
    }

    Logger::TRACE(TAG_FUNCTION_NAME, "Found {} functions.", Functions.size());
}
