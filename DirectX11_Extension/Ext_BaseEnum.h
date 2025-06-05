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

enum class MaterialType
{
    Static,
    StaticNonG,
    Dynamic,
    DynamicNonG,
    Debug,
    Unknown
};

enum class SamplerType
{
    LinearClamp,
    LinearMirror,
    LinearWrap,
    PointClamp,
    PointMirror,
    PointWrap,
    Unknown
};

enum class TextureType
{
    BaseColor,
    Normal,
    Roughness,
    Metalic,
    Emissive,
    Unknown
};



enum class ActorOrder
{
    Normal,
    Specific,
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