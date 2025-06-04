// cbuffer #1: TransformData (b0 ��������)
cbuffer TransformData : register(b0)
{
    float4 LocalPosition; // (������� ���� �� ����)
    float4 LocalRotation; // (������� ���� �� ����)
    float4 LocalScale; // (������� ���� �� ����)
    float4 LocalQuaternion; // (������� ���� �� ����)
    float4x4 LocalMatrix; // (������� ���� �� ����)

    float4 WorldPosition; // (������� ���� �� ����)
    float4 WorldRotation; // (������� ���� �� ����)
    float4 WorldQuaternion; // (������� ���� �� ����)
    float4 WorldScale; // (������� ���� �� ����)
    float4x4 WorldMatrix; // Actor(�޽�)�� ���� ��ȯ ���

    float4x4 ViewMatrix; // ī�޶� �� ���
    float4x4 ProjectionMatrix; // ī�޶� �������� ���
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
    float4 Position : POSITION; // �̹� float4
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL; // float4 �Է�
    uint4 BoneID : BONEID;
    float4 Weight : WEIGHT;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL; //������ float4�� ����
};

VSOutput Dynamic_VS(VSInput input)
{
    VSOutput output;

    float4x4 skinMatrix = float4x4(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    );

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        uint bIdx = input.BoneID[i];
        float w = input.Weight[i];
        if (w > 0.0f && bIdx < MAX_BONES)
        {
            skinMatrix += Bones[bIdx] * w;
        }
    }

    float4 skinnedPos;
    skinnedPos.x = dot(skinMatrix[0], input.Position); // row0 �� [x y z 1]
    skinnedPos.y = dot(skinMatrix[1], input.Position); // row1 �� [x y z 1]
    skinnedPos.z = dot(skinMatrix[2], input.Position); // row2 �� [x y z 1]
    skinnedPos.w = dot(skinMatrix[3], input.Position); // row3 �� [x y z 1]

    //output.Position = mul(skinnedPos, WorldViewProjectionMatrix);
    float4 worldPos = mul(skinnedPos, WorldMatrix); // -> ���� ��ǥ��(row ����)
    float4 viewPos = mul(worldPos, ViewMatrix); // -> �� ��ǥ��
    output.Position = mul(viewPos, ProjectionMatrix);
    
    float3 skinnedNormal3;
    {
        float3 row0 = float3(skinMatrix[0][0], skinMatrix[0][1], skinMatrix[0][2]);
        float3 row1 = float3(skinMatrix[1][0], skinMatrix[1][1], skinMatrix[1][2]);
        float3 row2 = float3(skinMatrix[2][0], skinMatrix[2][1], skinMatrix[2][2]);

        skinnedNormal3.x = dot(row0, input.Normal.xyz);
        skinnedNormal3.y = dot(row1, input.Normal.xyz);
        skinnedNormal3.z = dot(row2, input.Normal.xyz);
        skinnedNormal3 = normalize(skinnedNormal3);
    }

    float3 worldNormal3 = normalize(mul((float3x3) WorldMatrix, skinnedNormal3));

    output.Normal = float4(worldNormal3, 0.0f);
    output.Color = input.Color;
    output.TexCoord = float4(input.TexCoord.xy, 0.0f, 0.0f);

    return output;
}