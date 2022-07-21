#pragma once
#include "Generic.h"

using KeyCodeType = uint32_t;

struct vec2packed
{
    int16_t     x;
    int16_t     y;
};

enum class eInputType : uint64_t
{
    NONE = -1,
    KEYBOARD = 0xe57246c8b4be679b,
    GAMEPAD = 0x0bf2f460699f89a3
};

class InputInstance
{
    friend class InputInterface;
protected:
    InputInstance() = default;
    virtual ~InputInstance() = default;

    virtual uint32_t    GetKeyState(const KeyCodeType key) = 0;
    virtual uint32_t    GetMouseState(const KeyCodeType button) = 0;
    virtual void        Update() = 0;
};

class InputInterface
{
private:
    eInputType      InputType;
    bool            IsValid;
    InputInstance  *Instance;

public:
    InputInterface(const eInputType inputType, const WindowHandle windowHandle);

    ~InputInterface()
    {
        delete Instance;
    }

    inline const bool  Valid() const
    {
        return IsValid;
    }

    void Update();

    inline const bool   KeyPressed(const KeyCodeType keyCode) const
    {
        if (!Instance)
            return false;
        else
            if ((keyCode >= DIMOFS_BUTTON0 && keyCode <= DIMOFS_BUTTON7) || keyCode == DIMOFS_X || keyCode == DIMOFS_Y || keyCode == DIMOFS_Z)
                return Instance->GetMouseState(keyCode);
            else
                return Instance->GetKeyState(keyCode);
    }
};