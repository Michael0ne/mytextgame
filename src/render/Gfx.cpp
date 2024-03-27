#include "Gfx.h"
#include "DebugUI.h"

Gfx* Gfx::Instance;

namespace DebugUI
{
    extern void Update(const float_t delta);
}

void Gfx::Update(SDL_Renderer* renderer, const float delta)
{

    SDL_SetRenderDrawColor(renderer, ClearColor[0], ClearColor[1], ClearColor[2], SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    DebugUI::Update(delta);

    SDL_RenderPresent(renderer);
}

bool Gfx::Init(const WindowHandle windowHandle, const uint32_t width, const uint32_t height)
{
    std::cout << LOGGER_TAG << "Setup GFX device with resolution = " << width << " x " << height << std::endl;
    GfxWindowHandle = windowHandle;
    ClearColor[0] = 0;
    ClearColor[1] = 0;
    ClearColor[2] = 0;

    //  Add debug stuff.
    DebugUI::AddPanel("Gfx");
    DebugUI::TextItem::TextData textData{ "Set the clear color" };
    DebugUI::AddPanelItem("Gfx", DebugUI::Item::TEXT, &textData);
    DebugUI::SliderItem::SliderData sliderData{ DebugUI::SliderItem::ItemType::INT, "Clear Color", 3, ClearColor, (uint32_t)0, (uint32_t)255 };
    DebugUI::AddPanelItem("Gfx", DebugUI::Item::SLIDER, &sliderData);

    return true;
}

const WindowHandle Gfx::GetWindowHandle() const
{
    return GfxWindowHandle;
}