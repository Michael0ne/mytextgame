#pragma once
#include "Generic.h"

class Gfx
{
private:
    uint32_t    Width;
    uint32_t    Height;
    WindowHandle    GfxWindowHandle;

    static Gfx *Instance;

public:
    static Gfx& GetInstance()
    {
        if (!Instance)
            Instance = new Gfx;

        return *Instance;
    }

    void Update(SDL_Renderer* renderer, const float delta);
    bool Init(const WindowHandle windowHandle, const uint32_t width, const uint32_t height);
    const WindowHandle GetWindowHandle() const;
};