#pragma once
#include "Generic.h"

class Gfx
{
private:
    uint32_t    Width;
    uint32_t    Height;

    static Gfx *Instance;

public:
    static Gfx& GetInstance()
    {
        if (!Instance)
            Instance = new Gfx;

        return *Instance;
    }

    void Update(SDL_Renderer* renderer, const float delta);
    bool Init(const uint32_t width, const uint32_t height);
};