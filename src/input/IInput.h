#pragma once
#include "Generic.h"

struct vec2packed
{
    int16_t     x;
    int16_t     y;
};

enum class eInputType : HashType
{
    KEYBOARD = xxh64::hash("keyboard", 8, 0),
    GAMEPAD = xxh64::hash("gamepad", 7, 0)
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
    bool            IsValidInstance;
    InputInstance  *Instance;

public:
    InputInterface(const eInputType inputType, const WindowHandle windowHandle);

    ~InputInterface()
    {
        delete Instance;
    }

    inline const bool  IsValid() const
    {
        return IsValidInstance;
    }

    void Update();

    inline const bool   KeyPressed(const KeyCodeType keyCode) const
    {
        if (!Instance)
            return false;

        if (keyCode >= SDL_BUTTON_LEFT && keyCode <= SDL_BUTTON_RIGHT)
            return Instance->GetMouseState(keyCode);
        else
            return Instance->GetKeyState(keyCode);
    }
};