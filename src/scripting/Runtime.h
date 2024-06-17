#include "Generic.h"
#include "ScriptAsset.h"

namespace Scripting
{

    //  Scripting stuff.
    class Runtime
    {
    protected:
        static std::vector<uint32_t>    LoadedScripts;
        static std::string              LastError;

        static bool         RunScript(ScriptAsset& script, const std::string& functionName = "main");

    public:
        static bool         Start();
        static void         Stop();
        static void         Update(const float_t delta);
    };

}