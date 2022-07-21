#pragma once
#include "IInput.h"
#define _AMD64_
#include <Xinput.h>

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

    XINPUT_STATE    XInputState;

public:
    GamepadInput()
    {
        memset(&XInputState, NULL, sizeof(XINPUT_STATE));

        Buttons = NULL;
        LeftTrigger = NULL;
        RightTrigger = NULL;
        ThumbLeft = { 0, 0 };
        ThumbRight = { 0, 0 };
    }

    virtual ~GamepadInput()
    {
    }

    virtual uint32_t    GetKeyState(const KeyCodeType key) override
    {
        if (key < XINPUT_GAMEPAD_Y && key > XINPUT_GAMEPAD_DPAD_UP)
            return Buttons & key;
        else
            return NULL;
    }

    virtual uint32_t    GetMouseState(const KeyCodeType button) override
    {
        return NULL;
    }

    virtual void    Update() override
    {
        Buttons = NULL;
        LeftTrigger = NULL;
        RightTrigger = NULL;
        ThumbLeft = { 0, 0 };
        ThumbRight = { 0, 0 };

        if (!XInputGetState(0, &XInputState))
        {
            Buttons = XInputState.Gamepad.wButtons;
            LeftTrigger = XInputState.Gamepad.bLeftTrigger;
            RightTrigger = XInputState.Gamepad.bRightTrigger;

            ThumbLeft = { XInputState.Gamepad.sThumbLX, XInputState.Gamepad.sThumbLY };
            ThumbRight = { XInputState.Gamepad.sThumbRX, XInputState.Gamepad.sThumbRY };
        }
    }
};