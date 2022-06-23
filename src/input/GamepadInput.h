#pragma once
#include "IInput.h"

/// <summary>
/// Do not use this class directly!
/// InputInterface encapsulates it, so use that instead.
/// </summary>
class GamepadInput : public InputInstance
{
#define MAX_GAMEPAD_KEYS    12
#define MAX_STICKS_NUMBER   2

private:
    uint8_t     Buttons[MAX_GAMEPAD_KEYS];
    int8_t      SticksAxis[MAX_STICKS_NUMBER];

public:
    GamepadInput()
    {
        //  TODO: acquire dinput interface or whatever.
    }

    virtual ~GamepadInput()
    {
        //  TODO: release dinput interface.
    }

    virtual uint32_t    GetKeyState(const uint32_t key) override
    {
        return NULL;
    }

    virtual uint32_t    GetMouseState(const uint32_t button) override
    {
        return NULL;
    }
};