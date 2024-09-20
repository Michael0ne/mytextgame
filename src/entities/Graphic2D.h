#pragma once

#include "Generic.h"
#include "Node.h"

class Graphic2D : public Node
{
private:
    void* Texture;

public:
    Graphic2D();

    virtual ~Graphic2D() override;
};