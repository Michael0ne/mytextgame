#include "Generic.h"
#include "DebugUI.h"

class Scene
{
private:
    //std::string     Name;
    static char*    Name;
    void*           AssetPtr;

public:
    Scene() = default;

    //  A method to update the scene children logic.
    static void             Update(float_t timeDelta);

    //  A method to render all of the children.
    static void             Render(SDL_Renderer* renderer, float_t timeDelta);

    //  This will initialize a scene instance.
    static bool             Init();

    static void             Shutdown();
};