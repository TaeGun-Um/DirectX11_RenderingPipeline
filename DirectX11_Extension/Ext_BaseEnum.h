#pragma once

enum class CameraType
{
    None,
    Perspective,
    Orthogonal,
};

enum class ShaderType
{
    Vertex,
    Pixel,
    Geometry,
    Compute,
    Unknown
};