cbuffer TransformData : register(b0)
{
    float4 LocalPosition;
    float4 LocalRotation;
    float4 LocalScale;
    float4 LocalQuaternion;
    float4x4 LocalMatrix;

    float4 WorldPosition;
    float4 WorldRotation;
    float4 WorldQuaternion;
    float4 WorldScale;
    float4x4 WorldMatrix;

    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float4x4 WorldViewMatrix;
    float4x4 WorldViewProjectionMatrix;
}

struct VSInput
{
    float4 Position : POSITION;
    // float4 BLENDWEIGHT : BLENDWEIGHT;
    // int4 BLENDINDICES : BLENDINDICES;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 Position5 : POSITION;
};

VSOutput OrthogonalShadow_VS(VSInput _Input)
{
    VSOutput OutPut;
    
    //if (IsAnimation != 0)
    //{
    //    Skinning(InputPos, _Value.BLENDWEIGHT, _Value.BLENDINDICES, ArrAniMationMatrix);
    //    InputPos.w = 1.0f;
    //}
    
    OutPut.Position = mul(_Input.Position, WorldViewProjectionMatrix);
    OutPut.Position5 = OutPut.Position;
    OutPut.Position5.w = 1.0f;
    
    return OutPut;
}