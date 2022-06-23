#pragma once
#include "Generic.h"

enum class eInputType : uint64_t
{
    NONE = -1,
    KEYBOARD = 0xe57246c8b4be679b,
    GAMEPAD = 0x0bf2f460699f89a3
};

class InputInstance
{
public:
    InputInstance() = default;
    virtual ~InputInstance() = default;

    virtual uint32_t    GetKeyState(const uint32_t key) = 0;
    virtual uint32_t    GetMouseState(const uint32_t button) = 0;
};

class InputInterface
{
private:
    eInputType      InputType;
    bool            IsValid;
    InputInstance  *Instance;

public:
    InputInterface(const eInputType inputType);

    ~InputInterface()
    {
        delete Instance;
    }

    inline const bool  Valid() const
    {
        return IsValid;
    }

    void UpdateKeys(SDL_Event& inputevent);
    void UpdateMouse(SDL_Event& mouseevent);
};