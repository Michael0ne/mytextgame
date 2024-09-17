#pragma once

template <typename T>
struct vec3
{
    T X;
    T Y;
    T Z;
};

using vec3f = vec3<float_t>;
using vec3i = vec3<int32_t>;

template <typename T>
struct vec4
{
    T X;
    T Y;
    T Z;
    T W;
};

using vec4f = vec4<float_t>;
using vec4i = vec4<int32_t>;