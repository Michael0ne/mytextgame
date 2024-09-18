/*
* File: CameraController.h
* Purpose: your one stop shop for all things related to camera controller. This class will handle user's input and update all engine cameras accordingly.
*/
#include "Generic.h"
#include "Types.h"
#include "input/IInput.h"

class CameraController
{
private:
    static vec4f    Position;
    static float_t  Velocity;

public:
    static bool Init();
    static void Shutdown();

    static void Update(InputInterface* input, const float_t timeDelta);

    static inline const vec4f& GetPosition()
    {
        return Position;
    }
};