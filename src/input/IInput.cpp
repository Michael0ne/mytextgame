#include "IInput.h"
#include "KeyboardInput.h"
#include "GamepadInput.h"

InputInterface::InputInterface(const eInputType inputType, const WindowHandle windowHandle)
{
    IsValid = true;
    InputType = inputType;
    Instance = nullptr;

    switch (inputType)
    {
    case eInputType::KEYBOARD:
        Instance = new KeyboardInput(windowHandle);
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
void InputInterface::Update()
{
    if (!Instance)
        return;

    Instance->Update();
}