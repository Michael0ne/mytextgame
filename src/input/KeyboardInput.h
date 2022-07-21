#pragma once
#include "IInput.h"

/// <summary>
/// Do not use this class directly!
/// InputInterface encapsulates it, so use that instead.
/// </summary>
class KeyboardInput : public InputInstance
{
    #define MAX_KEYBOARD_KEYS   256
    #define MAX_MOUSE_BUTTONS   8

private:
    BYTE        Keys[MAX_KEYBOARD_KEYS];
    uint8_t     MouseButtons[MAX_MOUSE_BUTTONS];
    LONG        MouseX;
    LONG        MouseY;
    LONG        MouseWheel;

    LPDIRECTINPUT8          DInputInterface;
    LPDIRECTINPUTDEVICE8    KeyboardInputDevice;
    LPDIRECTINPUTDEVICE8    MouseInputDevice;

public:
    KeyboardInput(const WindowHandle windowHandle)
    {
        if (!windowHandle)
            DebugBreak();

        memset(Keys, NULL, MAX_KEYBOARD_KEYS);
        memset(MouseButtons, NULL, MAX_MOUSE_BUTTONS);

        DInputInterface = nullptr;
        KeyboardInputDevice = nullptr;
        MouseInputDevice = nullptr;

        if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&DInputInterface, NULL)))
            throw new std::exception("Interface for DirectInput could not be obtained!");

        if (FAILED(DInputInterface->CreateDevice(GUID_SysKeyboard, &KeyboardInputDevice, NULL)))
            throw new std::exception("Keyboard device could not be created!");

        if (FAILED(DInputInterface->CreateDevice(GUID_SysMouse, &MouseInputDevice, NULL)))
            throw new std::exception("Mouse device could not be created!");

        if (FAILED(KeyboardInputDevice->SetDataFormat(&c_dfDIKeyboard)))
            throw new std::exception("Can't set keyboard data format!");

        if (FAILED(MouseInputDevice->SetDataFormat(&c_dfDIMouse2)))
            throw new std::exception("Can't set mouse data format!");

        if (FAILED(KeyboardInputDevice->SetCooperativeLevel(windowHandle, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
            throw new std::exception("Can't set keyboard cooperative level!");

        if (FAILED(MouseInputDevice->SetCooperativeLevel(windowHandle, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
            throw new std::exception("Can't set mouse cooperative level!");

        DIDEVCAPS DIKeyboardCaps = {};
        DIDEVCAPS DIMouseCaps = {};
        DIKeyboardCaps.dwSize = sizeof(DIDEVCAPS);
        DIMouseCaps.dwSize = sizeof(DIDEVCAPS);

        if (FAILED(KeyboardInputDevice->GetCapabilities(&DIKeyboardCaps)))
            throw new std::exception("Could not get keyboard capabilities!");

        if (FAILED(MouseInputDevice->GetCapabilities(&DIMouseCaps)))
            throw new std::exception("Could not get mouse device capabilities!");

        if (!(DIKeyboardCaps.dwFlags & DIDC_ATTACHED))
            throw new std::exception("Keyboard is not attached!");

        if (!(DIMouseCaps.dwFlags & DIDC_ATTACHED))
            throw new std::exception("Mouse is not attached!");

        KeyboardInputDevice->Acquire();
        MouseInputDevice->Acquire();
    }

    virtual ~KeyboardInput()
    {
        if (MouseInputDevice)
        {
            MouseInputDevice->Unacquire();
            MouseInputDevice->Release();
        }

        if (KeyboardInputDevice)
        {
            KeyboardInputDevice->Unacquire();
            KeyboardInputDevice->Release();
        }

        if (DInputInterface)
            DInputInterface->Release();
    }

    virtual uint32_t    GetKeyState(const KeyCodeType key) override
    {
        if (key > MAX_KEYBOARD_KEYS)
            return NULL;
        else
            return Keys[key] & 0x80;
    }

    virtual uint32_t    GetMouseState(const KeyCodeType button) override
    {
        if (button > MAX_MOUSE_BUTTONS)
            return NULL;
        else
            return MouseButtons[button] & 0x80;
    }

    virtual void    Update() override
    {
        if (FAILED(KeyboardInputDevice->GetDeviceState(MAX_KEYBOARD_KEYS, Keys)))
        {
            memset(Keys, NULL, MAX_KEYBOARD_KEYS);

            HRESULT keyboardAcquireResult = KeyboardInputDevice->Acquire();
            while (keyboardAcquireResult == DIERR_INPUTLOST)
                keyboardAcquireResult = KeyboardInputDevice->Acquire();

            return;
        }

        DIMOUSESTATE2 DIMouseState = {};
        if (FAILED(MouseInputDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &DIMouseState)))
        {
            memset(MouseButtons, NULL, MAX_MOUSE_BUTTONS);

            HRESULT mouseAcquireResult = MouseInputDevice->Acquire();
            while (mouseAcquireResult == DIERR_INPUTLOST)
                mouseAcquireResult = MouseInputDevice->Acquire();

            return;
        }

        memcpy(MouseButtons, DIMouseState.rgbButtons, MAX_MOUSE_BUTTONS);
        MouseX = DIMouseState.lX;
        MouseY = DIMouseState.lY;
        MouseWheel = DIMouseState.lZ;
    }
};