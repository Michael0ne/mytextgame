#include "Gfx.h"

Gfx* Gfx::Instance;

void Gfx::Update(SDL_Renderer* renderer, const float delta)
{
}

bool Gfx::Init(const uint32_t width, const uint32_t height)
{
    std::cout << LOGGER_TAG << "Setup GFX device with resolution = " << width << " x " << height << std::endl;

    return true;
}