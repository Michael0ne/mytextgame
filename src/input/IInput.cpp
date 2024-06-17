#include "IInput.h"
#include "KeyboardInput.h"
#include "GamepadInput.h"
#include "Logger.h"

InputInterface::InputInterface(const eInputType inputType, const WindowHandle windowHandle)
{
    IsValidInstance = true;
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
        IsValidInstance = false;
        break;
    }

    Logger::TRACE(TAG_FUNCTION_NAME, "IsValidInstance = {}.", IsValidInstance);
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