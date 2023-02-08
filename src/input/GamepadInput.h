#pragma once
#include "IInput.h"

/// <summary>
/// Do not use this class directly!
/// InputInterface encapsulates it, so use that instead.
/// An abstraction for managing XBox360 controller, using XInput.
/// </summary>
class GamepadInput : public InputInstance
{
private:
    uint16_t    Buttons;
    uint8_t     LeftTrigger;
    uint8_t     RightTrigger;

    vec2packed  ThumbLeft;
    vec2packed  ThumbRight;

    SDL_Gamepad*    GamepadObject;

public:
    GamepadInput()
    {
        Buttons = 0;
        LeftTrigger = 0;
        RightTrigger = 0;
        ThumbLeft = { 0, 0 };
        ThumbRight = { 0, 0 };

        //TODO: enumerate gamepads and init GamepadObject.
    }

    virtual ~GamepadInput()
    {
        //TODO: close active GamepadObject instance.
    }

    virtual uint32_t    GetKeyState(const KeyCodeType key) override
    {
        //TODO: get active GamepadObject key state.
        return 0;
    }

    virtual uint32_t    GetMouseState(const KeyCodeType button) override
    {
        return 0;
    }

    virtual void    Update() override
    {
        Buttons = 0;
        LeftTrigger = 0;
        RightTrigger = 0;
        ThumbLeft = { 0, 0 };
        ThumbRight = { 0, 0 };

        //TODO: update active GamepadObject state and see if it's still alive. Handle disconnect.
    }
};