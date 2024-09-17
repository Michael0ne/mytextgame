#include "Generic.h"
#include "Types.h"

#include <glm/glm.hpp>

class Camera
{
protected:
    std::string     Name;

public:
    Camera(const std::string& name)
        :Name(std::move(name))
    {}

    virtual ~Camera() = 0;
};

class OrthoCamera : public Camera
{
private:
    glm::mat4x4     Model;
    glm::mat4x4     View;
    glm::mat4x4     Projection;

    glm::vec4       Rotation;
    glm::vec4       Position;
    glm::float32_t  FOV;
    glm::float32_t  AspectRatio;

public:
    OrthoCamera(const float_t fov)
        :Camera("Orthographic")
    {
        FOV = fov;
        Position = { 0.f, 0.f, 0.f, 0.f };
    }

    void    Update(const float_t delta);
};