#pragma once

#include "Generic.h"
#include "Types.h"

class Node
{
private:
    vec4f       Position;
    vec4f       Orientation;

    std::string Name;

public:
    Node() = default;

    virtual ~Node() = default;

    virtual vec4f GetBounds() const;
    virtual const vec4f& GetPosition() const;
    virtual const vec4f& GetOrientation() const;

    virtual void Render();
    virtual void UpdateLogic();
};