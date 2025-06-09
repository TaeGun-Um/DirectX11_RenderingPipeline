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
    Static,                       // 스태틱 메시 세팅
    Dynamic,                  // 다이나믹메시 세팅
    PBR,                         // 텍스쳐 여러개 처리용
    DeferredLight,           // DefferedUnit용
    DeferredPost,            // DefferedPostUnit용
    DeferredMerge,         // DefferedMergeUnit용
    RenderTargetMerge,  // RenderTergetUnit용
    Debug,                     // 와이어 프레임으로 그리기용
    NonGStatic,              // 그래픽스 세팅X 스태틱, 이제 안씀
    NonGDynamic,          // 그래픽스 세팅X 다이나믹, 이제 안씀
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