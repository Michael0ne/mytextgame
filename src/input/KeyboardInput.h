#pragma once
#include "IInput.h"

/// <summary>
/// Do not use this class directly!
/// InputInterface encapsulates it, so use that instead.
/// </summary>
class KeyboardInput : public InputInstance
{
    #define MAX_KEYBOARD_KEYS   256
    #define MAX_MOUSE_BUTTONS   6

private:
    uint8_t     Keys[MAX_KEYBOARD_KEYS];
    uint8_t     MouseButtons[MAX_MOUSE_BUTTONS];

public:
    KeyboardInput()
    {
        //  TODO: acquire dinput interface or whatever.
    }

    virtual ~KeyboardInput()
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