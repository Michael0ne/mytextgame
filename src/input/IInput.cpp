#include "IInput.h"
#include "KeyboardInput.h"
#include "GamepadInput.h"

InputInterface::InputInterface(const eInputType inputType)
{
    IsValid = true;
    InputType = inputType;
    Instance = nullptr;

    switch (inputType)
    {
    case eInputType::KEYBOARD:
        Instance = new KeyboardInput;
        break;
    case eInputType::GAMEPAD:
        Instance = new GamepadInput;
        break;
    default:
        IsValid = false;
        break;
    }

    std::cout << LOGGER_TAG << "IsValid = " << IsValid << std::endl;
}

/// <summary>
/// Updates underlying struct's keys (keys for keyboard, buttons for gamepad).
/// </summary>
/// <param name="inputevent">Reference to SDL event struct.</param>
void InputInterface::UpdateKeys(SDL_Event& inputevent)
{

}

/// <summary>
/// Updates underlying struct's data (mouse data for mouse, sticks for gamepad).
/// </summary>
/// <param name="mouseevent">Reference to SDL event struct.</param>
void InputInterface::UpdateMouse(SDL_Event& mouseevent)
{
}