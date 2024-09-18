#include "CameraController.h"
#include "Logger.h"
#include "DebugUI.h"

vec4f CameraController::Position = { 0, 0, 0, 0 };
float_t CameraController::Velocity = 800.f;

void CameraController::Update(InputInterface* input, const float_t timeDelta)
{
    if (input->KeyPressed(SDL_SCANCODE_LEFT) || input->KeyPressed(SDL_SCANCODE_A))
    {
        Position.X -= Velocity * timeDelta;
    }

    if (input->KeyPressed(SDL_SCANCODE_RIGHT) || input->KeyPressed(SDL_SCANCODE_D))
    {
        Position.X += Velocity * timeDelta;
    }

    if (input->KeyPressed(SDL_SCANCODE_UP) || input->KeyPressed(SDL_SCANCODE_W))
    {
        Position.Y -= Velocity * timeDelta;
    }

    if (input->KeyPressed(SDL_SCANCODE_DOWN) || input->KeyPressed(SDL_SCANCODE_S))
    {
        Position.Y += Velocity * timeDelta;
    }
}

bool CameraController::Init()
{
    DebugUI::AddPanel("Camera");

    DebugUI::AddPanelItem("Camera", DebugUI::Item::SLIDER, DebugUI::SliderItem::SliderData<float_t>(DebugUI::SliderItem::ItemType::FLOAT, "Position", 2, (float_t*)&Position, 0.f, 800.f));
    DebugUI::AddPanelItem("Camera", DebugUI::Item::SLIDER, DebugUI::SliderItem::SliderData<float_t>(DebugUI::SliderItem::ItemType::FLOAT, "Velocity", 1, &Velocity, 0.f, 1600.f));

    return true;
}

void CameraController::Shutdown()
{

}