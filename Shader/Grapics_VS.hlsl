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
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL;
};

VSOutput Grapics_VS(VSInput _Input)
{
    VSOutput Output;
    // _Input.Position.w = 1.0f;
    
    float4 WorldPos = mul(_Input.Position, WorldMatrix);
    float4 ViewPos = mul(WorldPos, ViewMatrix);
    Output.Position = mul(ViewPos, ProjectionMatrix);
    // Output.Position = mul(_Input.Position, WorldViewProjectionMatrix);
    
    Output.Color = _Input.Color;
    Output.TexCoord = _Input.TexCoord;
    
    // Output.Normal = _Input.Normal;
    // 2) ���� ����(_Input.Normal.xyz)�� �������� ������ �������� ��ȯ
    //    (���⼭ ����� ������ ���ٴ� ���� �Ͽ� WorldMatrix*ViewMatrix ���� 3��3�� ���)
    float4x4 worldView4x4 = mul(WorldMatrix, ViewMatrix);
    float3x3 worldView3x3 = (float3x3) worldView4x4;
    float3 normalVS3 = normalize(mul(_Input.Normal.xyz, worldView3x3));

    // 3) float3 �� float4 �� ����� Ȯ��. w ������ ���� �����̹Ƿ� 0.0f �� ä��ϴ�.
    Output.Normal = float4(normalVS3, 0.0f);
   
    return Output;
}