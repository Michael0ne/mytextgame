#pragma once
#include "Generic.h"

struct GfxResource
{
    void* AssetBufferPtr;
    GfxResource* Parent;
    std::string Name;

    struct
    {
        uint32_t    Width;
        uint32_t    Height;
        uint32_t    BPP;
        uint32_t    Format;

        bool        HasSubResource;
    }   MetaData;

    GfxResource(void* resBufferPtr)
    {
        //  TODO: read resource buffer and get metadata from there.
    }
};

class Gfx
{
private:
    uint32_t    Width;
    uint32_t    Height;
    WindowHandle    GfxWindowHandle;
    uint32_t     ClearColor[3];

    SDL_Renderer* SDLRenderer = nullptr;

    static Gfx *Instance;

    void RenderSampleRect();

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