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
    Static, // 그래픽스를 사용하는 스태틱 세팅
    StaticNonG, // 그래픽스를 사용하지 않는 스태틱 세팅
    Dynamic, // 그래픽스를 사용하는 다이나믹 세팅
    DynamicNonG, // 그래픽스를 사용하지 않는 다이나믹 세팅
    Debug, // 와이어 프레임으로 그리기용
    PBR, // 여러 텍스쳐를 처리할 수 있는 머티리얼 세팅
    Merge, // RenderTerget Unit용 머티리얼 세팅
    DeferredLight, // Deffered(Camera Light 계산용)용 머티리얼 세팅
    DeferredShadow, // Deffered(Camera Light 계산용)용 머티리얼 세팅
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
    Specular, // Metalic
    AmbientOcclusion,
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