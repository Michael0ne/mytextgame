#include "Gfx.h"
#include "DebugUI.h"
#include "Logger.h"
#include "input/CameraController.h"
#include "entities/Scene.h"

Gfx* Gfx::Instance;

namespace DebugUI
{
    extern void Update(const float_t delta);
}

void Gfx::Update(SDL_Renderer* renderer, const float delta)
{
    SDLRenderer = renderer;

    SDL_SetRenderDrawColor(renderer, ClearColor[0], ClearColor[1], ClearColor[2], SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    DebugUI::Update(delta);
    Scene::Render(renderer, delta);

    SDL_RenderPresent(renderer);
}

bool Gfx::Init(const WindowHandle windowHandle, const uint32_t width, const uint32_t height)
{
    Logger::TRACE(TAG_FUNCTION_NAME, "Setup GFX device with resolution = {}x{}.", width, height);

    GfxWindowHandle = windowHandle;
    ClearColor[0] = 0;
    ClearColor[1] = 0;
    ClearColor[2] = 0;

    //  Add debug stuff.
    DebugUI::AddPanel("Gfx");

    DebugUI::AddPanelItem("Gfx", DebugUI::Item::TEXT, DebugUI::TextItem::TextData("Set the clear color"));
    DebugUI::AddPanelItem("Gfx", DebugUI::Item::SLIDER, DebugUI::SliderItem::SliderData<uint32_t>(DebugUI::SliderItem::ItemType::INT, "Clear Color", (uint32_t)3, ClearColor, (uint32_t)0, (uint32_t)255));

    return true;
}

const WindowHandle Gfx::GetWindowHandle() const
{
    return GfxWindowHandle;
}