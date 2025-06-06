// cbuffer #1: TransformData (b0 레지스터)
cbuffer TransformData : register(b0)
{
    float4 LocalPosition; // (사용하지 않을 수 있음)
    float4 LocalRotation; // (사용하지 않을 수 있음)
    float4 LocalScale; // (사용하지 않을 수 있음)
    float4 LocalQuaternion; // (사용하지 않을 수 있음)
    float4x4 LocalMatrix; // (사용하지 않을 수 있음)

    float4 WorldPosition; // (사용하지 않을 수 있음)
    float4 WorldRotation; // (사용하지 않을 수 있음)
    float4 WorldQuaternion; // (사용하지 않을 수 있음)
    float4 WorldScale; // (사용하지 않을 수 있음)
    float4x4 WorldMatrix; // Actor(메시)의 월드 변환 행렬

    float4x4 ViewMatrix; // 카메라 뷰 행렬
    float4x4 ProjectionMatrix; // 카메라 프로젝션 행렬
    float4x4 WorldViewMatrix; // WorldMatrix * ViewMatrix
    float4x4 WorldViewProjectionMatrix; // WorldMatrix * ViewMatrix * ProjectionMatrix
};

#define MAX_BONES 100
cbuffer CB_SkinnedMatrix : register(b1)
{
    float4x4 Bones[MAX_BONES];
};

struct VSInput
{
    float4 Position : POSITION;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
    uint4 BoneID : BONEID;
    float4 Weight : WEIGHT;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 TexCoord : TEXCOORD;
    float3 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
};

VSOutput Dynamic_VS(VSInput _Input)
{
    VSOutput Output;

    float4x4 SkinMatrix = float4x4(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    );

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        uint bIdx = _Input.BoneID[i];
        float w = _Input.Weight[i];
        if (w > 0.0f && bIdx < MAX_BONES)
        {
            SkinMatrix += Bones[bIdx] * w;
        }
    }

    float4 SkinnedPos;
    SkinnedPos.x = dot(SkinMatrix[0], _Input.Position); // row0 · [x y z 1]
    SkinnedPos.y = dot(SkinMatrix[1], _Input.Position); // row1 · [x y z 1]
    SkinnedPos.z = dot(SkinMatrix[2], _Input.Position); // row2 · [x y z 1]
    SkinnedPos.w = dot(SkinMatrix[3], _Input.Position); // row3 · [x y z 1]

    //Output.Position = mul(skinnedPos, WorldViewProjectionMatrix);
    float4 WorldPos = mul(SkinnedPos, WorldMatrix); // -> 월드 좌표계(row 형태)
    Output.WorldNormal = WorldPos;
    float4 ViewPos = mul(WorldPos, ViewMatrix); // -> 뷰 좌표계
    Output.Position = mul(ViewPos, ProjectionMatrix);
    
    float3 SkinnedNormal;
    {
        float3 row0 = float3(SkinMatrix[0][0], SkinMatrix[0][1], SkinMatrix[0][2]);
        float3 row1 = float3(SkinMatrix[1][0], SkinMatrix[1][1], SkinMatrix[1][2]);
        float3 row2 = float3(SkinMatrix[2][0], SkinMatrix[2][1], SkinMatrix[2][2]);

        SkinnedNormal.x = dot(row0, _Input.Normal.xyz);
        SkinnedNormal.y = dot(row1, _Input.Normal.xyz);
        SkinnedNormal.z = dot(row2, _Input.Normal.xyz);
        SkinnedNormal = normalize(SkinnedNormal);
    }

    float3 WorldNormal = normalize(mul((float3x3) WorldMatrix, SkinnedNormal));

    Output.WorldNormal = WorldNormal;
    Output.TexCoord = float4(_Input.TexCoord.xy, 0.0f, 0.0f);

    return Output;
}