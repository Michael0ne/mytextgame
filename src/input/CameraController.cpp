#include "CameraController.h"
#include "Logger.h"
#include "DebugUI.h"

#define CAMERA_MOVEMENT_SPEED 10000.f

vec4f CameraController::Position = { 0, 0, 0, 0 };

void CameraController::Update(InputInterface* input, const float_t timeDelta)
{
    if (input->KeyPressed(SDL_SCANCODE_LEFT) || input->KeyPressed(SDL_SCANCODE_A))
    {
        Position.X -= CAMERA_MOVEMENT_SPEED * timeDelta;
    }

    if (input->KeyPressed(SDL_SCANCODE_RIGHT) || input->KeyPressed(SDL_SCANCODE_D))
    {
        Position.X += CAMERA_MOVEMENT_SPEED * timeDelta;
    }

    if (input->KeyPressed(SDL_SCANCODE_UP) || input->KeyPressed(SDL_SCANCODE_W))
    {
        Position.Y -= CAMERA_MOVEMENT_SPEED * timeDelta;
    }

    if (input->KeyPressed(SDL_SCANCODE_DOWN) || input->KeyPressed(SDL_SCANCODE_S))
    {
        Position.Y += CAMERA_MOVEMENT_SPEED * timeDelta;
    }
}

bool CameraController::Init()
{
    DebugUI::AddPanel("Camera");

    DebugUI::AddPanelItem("Camera", DebugUI::Item::SLIDER, DebugUI::SliderItem::SliderData<float_t>(DebugUI::SliderItem::ItemType::FLOAT, "Position", 4, (float_t*)&Position, 0.f, 800.f));

    return true;
}

void CameraController::Shutdown()
{

}