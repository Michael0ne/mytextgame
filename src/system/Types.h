template <typename T>
struct vec3
{
    T X;
    T Y;
    T Z;
};

using vec3f = vec3<float_t>;
using vec3i = vec3<int32_t>;