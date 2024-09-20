#include "Node.h"

vec4f Node::GetBounds() const
{
    return {
        0.f, 0.f, 0.f, 0.f
    };
}

const vec4f& Node::GetPosition() const
{
    return Position;
}

const vec4f& Node::GetOrientation() const
{
    return Orientation;
}

void Node::Render()
{
}

void Node::UpdateLogic()
{
}
