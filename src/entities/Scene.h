#pragma once

#include "Generic.h"
#include "DebugUI.h"

class Node;

class Scene
{
private:
    static char*            Name;
    static void*            AssetPtr;

    static std::vector<Node*>   Nodes;

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