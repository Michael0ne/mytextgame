#include "Generic.h"

//  Scripting stuff.
namespace Script
{
    typedef uint64_t            EntityHandle;
    typedef uint64_t            ScriptHandle;
    typedef std::string&        StringArgument;
    typedef uint32_t            IntArgument;

    #define SCRIPT_ENTRY_FUNCTION "main"
    #define ENTITY_HANDLE_UNDEFINED ((EntityHandle)-1)

    struct Variable
    {
        std::string     Name;
        std::string     FunctionName;
        void*           Value;
        uint32_t        References;
    };

    typedef Variable    FunctionVariable;
    typedef Variable    FunctionArgument;

    struct FunctionInformation
    {
        std::string     Name;
        std::list<Variable> Arguments;
        std::list<FunctionVariable> Variables;
    };

    struct Events
    {
        enum class eEventId
        {
            EVENT_UNDEFINED = -1,
            //  Input-related events.
            EVENT_CLICK = 100,
            EVENT_DOUBLE_CLICK = 101,
            EVENT_MOUSEMOVE = 102,
            //  Draw-related events.
            EVENT_DRAW = 200,
            //  Game logic events.
        };

    private:
        static bool CanAttachEvent(EntityHandle entityHandle, eEventId eventId);

    public:
        static bool AddEvent(EntityHandle entityHandle, eEventId eventId, void (*eventHandler)());
        static bool FireEvent(EntityHandle entityHandle, eEventId eventId);
    };

    struct Gfx
    {
        static void FadeOut(IntArgument duration);
        static void FadeIn(IntArgument duration);
    };

    struct Scene
    {
        static std::string GetScene();
        static void SetScene(StringArgument scenePath);
        static void Unload(EntityHandle entityHandle);
    };

    struct Resources
    {
        static std::string GetScriptName();
        static ScriptHandle StartScript(StringArgument scriptPath);
    };

    struct Entity
    {
        static EntityHandle GetEntityByName(StringArgument entityName);
    };
}