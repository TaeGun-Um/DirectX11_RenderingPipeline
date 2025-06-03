#pragma once

enum class ShaderType
{
    Vertex,
    Pixel,
    Geometry,
    Compute,
    Unknown
};

enum class RenderPath
{
    Forward,
    Deferred,
    Alpha,
    Unknown
};

enum class CollsionType
{
    Sphere3D,
    AABB3D,
    OBB3D,
    Unknown,
};

enum class CollisionGroup
{
    Player,
    PlayerAttack,
    Platform,
    Wall,
    Box,
    Unknown
};