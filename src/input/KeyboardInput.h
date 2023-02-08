#pragma once
#include "IInput.h"

/// <summary>
/// Do not use this class directly!
/// InputInterface encapsulates it, so use that instead.
/// </summary>
class KeyboardInput : public InputInstance
{
private:
    const uint8_t    *Keys;
    union
    {
        struct
        {
            unsigned    LeftButton;
            unsigned    MiddleButton;
            unsigned    RightButton;
            unsigned    X1Button;
            unsigned    X2Button;
        }           StateBits;
        uint32_t    State;
    }           MouseState;
    float       MousePosX;
    float       MousePosY;
    uint32_t    MouseWheel;

public:
    KeyboardInput(const WindowHandle windowHandle)
    {
        Keys = nullptr;
        MouseState.State = 0;
        MousePosX = 0.f;
        MousePosY = 0.f;
        MouseWheel = 0;
    }

    virtual ~KeyboardInput()
    {
    }

    virtual uint32_t    GetKeyState(const KeyCodeType key) override
    {
        return (Keys[key] & 1);
    }

    virtual uint32_t    GetMouseState(const KeyCodeType button) override
    {
        return (MouseState.State & SDL_BUTTON(button));
    }

    //  This will only update the keyboard state and mouse state.
    virtual void    Update() override
    {
        MouseState.State = SDL_GetMouseState(&MousePosX, &MousePosY);
        Keys = SDL_GetKeyboardState(nullptr);
    }
};