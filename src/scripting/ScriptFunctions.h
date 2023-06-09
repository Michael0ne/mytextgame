#include "Generic.h"

//  Scripting stuff.
class ScriptEngine
{
private:
    static std::vector<uint32_t>    LoadedScripts;

public:
    static bool         Start();
    static void         Stop();
    static void         Update(const float_t delta);
};