#pragma once

enum class ProjectionType
{
    Perspective,
    Orthogonal,
    Unknown
};

enum class ShaderType
{
    Vertex,
    Pixel,
    Geometry,
    Compute,
    Unknown
};

enum class SortType
{
    ZSort,
    YSort,
    Unknown
};

enum class RenderPath
{
    Forward,
    Deferred,
    Alpha,
    Unknown
};