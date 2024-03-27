#include "ScriptAsset.h"

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
                    std::cerr << LOGGER_TAG << "Syntax Parse Error: empty argument #" << argumentIndex << " name in '" << functionName << "' function arguments list." << std::endl;
                    ErrorsFound++;
                    argumentIndex++;
                    continue;
                }

                if (token == "function")
                {
                    std::cerr << LOGGER_TAG << "Syntax Parse Error: unexpected usage of keyword as #" << argumentIndex << " '" << functionName << "' function argument." << std::endl;
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
                    std::cerr << LOGGER_TAG << "Syntax Parse Error: empty function '" << functionName << "' call argument #" << argumentIndex << ", called from '" << calleeFunctionName << "'." << std::endl;
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
                    std::cerr << LOGGER_TAG << "Syntax Parse Error: unexpected token found in '" << functionName << "' function call, called from '" << calleeFunctionName << "'." << std::endl;
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
                std::cerr << LOGGER_TAG << "Syntax Parse Error: unexpected include keyword found inside function body." << std::endl;
                ErrorsFound++;
                return;
            }

            auto quotedString = currentTokenString.substr(currentTokenString.find_first_of('"') + 1);
            quotedString = quotedString.substr(0, quotedString.find_first_of('"') - 1);

            if (!quotedString.length())
            {
                std::cerr << LOGGER_TAG << "Syntax Parse Error: empty include directive found." << std::endl;
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
                std::cerr << LOGGER_TAG << "Syntax Parse Error: unexpected function keyword found inside function body." << std::endl;
                ErrorsFound++;
                return;
            }

            //  Malformed function definition?
            if (currentTokenString.find_first_of('(') == std::string::npos || currentTokenString.find_first_of(')') == std::string::npos)
            {
                std::cerr << LOGGER_TAG << "Syntax Parse Error: malformed function definition! Can't find arguments list." << std::endl;
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

            FunctionDefinition thisFunction;
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
            const auto functionRef = std::find_if(Functions.begin(), Functions.end(), [&](const FunctionDefinition& f) { return f.Name == calledFunctionName; });
            if (functionRef == std::end(Functions))
            {
                std::cerr << LOGGER_TAG << "Syntax Parse Error: function '" << calledFunctionName << "' called from '" << ParserState.Context.FunctionName << "' was not found." << std::endl;
                ErrorsFound++;
                return;
            }

            //  Parse arguments list.
            std::vector<std::string> argumentsList;
            ParseArgumentsListCall(calledFunctionArguments, calledFunctionName, ParserState.Context.FunctionName, argumentsList);

            //  Check if number of arguments that's passed in called function match the expected number of arguments for that function.
            if (argumentsList.size() != functionRef->Arguments.size())
            {
                std::cerr << LOGGER_TAG << "Syntax Parse Error: function's '" << calledFunctionName << "' number of arguments doesn't match the function prototype, called from '" << ParserState.Context.FunctionName << "'." << std::endl;
                ErrorsFound++;
                return;
            }

            auto currentFunction = std::find_if(Functions.begin(), Functions.end(), [&](const FunctionDefinition& f) { return ParserState.Context.FunctionName == f.Name; });

            currentFunction->ControlFlow.push_back({ ControlFlowElement::FUNCTION_CALL, (size_t)std::distance(Functions.begin(), functionRef) });

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
                std::cerr << LOGGER_TAG << "Syntax Parse Error: function's '" << ParserState.Context.FunctionName << "' condition statement is missing it's body." << std::endl;
                ErrorsFound++;
                return;
            }

            auto currentFunction = std::find_if(Functions.begin(), Functions.end(), [&](const FunctionDefinition& f) { return ParserState.Context.FunctionName == f.Name; });

            currentFunction->ControlFlow.push_back({ ControlFlowElement::CONDITION_START, (size_t)-1 });

            continue;
        }

        //  Condition statement ends?
        if (currentTokenString.starts_with("endif") && ParserState.IsCondition && ParserState.IsFunctionBody)
        {
            ParserState.IsCondition = false;

            auto currentFunction = std::find_if(Functions.begin(), Functions.end(), [&](const FunctionDefinition& f) { return ParserState.Context.FunctionName == f.Name; });

            currentFunction->ControlFlow.push_back({ ControlFlowElement::CONDITION_END, (size_t)-1 });

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

            auto currentFunction = std::find_if(Functions.begin(), Functions.end(), [&](const FunctionDefinition& f) { return ParserState.Context.FunctionName == f.Name; });

            if (!varName.length() || !varRightSide.length())
            {
                std::cerr << LOGGER_TAG << "Syntax Parse Error: malformed '" << currentFunction->Name << "' function's variable syntax." << std::endl;
                ErrorsFound++;
                return;
            }

            currentFunction->Variables.push_back({ varName, varRightSide });
            currentFunction->ControlFlow.push_back({ ControlFlowElement::VARIABLE_ASSIGNMENT, currentFunction->Variables.size() - 1 });

            continue;
        }

        //  None of the above. Unknown string.
        std::cerr << LOGGER_TAG << "Syntax Parse Error: unknown token found '" << currentTokenString << "'." << std::endl;
        ErrorsFound++;
    }

    std::cout << LOGGER_TAG << "Found " << Functions.size() << " functions." << std::endl;
}
