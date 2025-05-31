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

VSOutput Animation_VS(VSInput input)
{
    VSOutput output;

    //==========================================================
    // (1) ��Ų ���(skinMatrix) ���: �� �迭(Bones[])��
    //     ���� BoneID/Weight�� ����ġ �ջ�(����)�Ͽ� ����
    //==========================================================
    float4x4 skinMatrix = float4x4(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    );

    // Bones[bIdx]�� �̹� "�۷ι� ����� �� �ִϸ��̼� ���� ��� �� OffsetMatrix" ���·� ����
    // (Ext_FBXAnimator.RenderSkinnedMesh()���� CB_SkinnedMatrix�� ���޵�)
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

    //==========================================================
    // (2) Position ��Ű��: �� ���ε����� ���� ����(input.Position)
    //     �� skinMatrix�� ��ȯ
    //==========================================================
    // HLSL���� ��� �ε��� ������ skinMatrix[row][col] ����
    float4 skinnedPos;
    skinnedPos.x = dot(skinMatrix[0], input.Position); // ��(row) 0 �� ����
    skinnedPos.y = dot(skinMatrix[1], input.Position); // �� 1 �� ����
    skinnedPos.z = dot(skinMatrix[2], input.Position); // �� 2 �� ����
    skinnedPos.w = dot(skinMatrix[3], input.Position); // �� 3 �� ����

    //==========================================================
    // (3) �� ���� �� ���� ���� ��ȯ
    //==========================================================
    float4 worldPos = mul(WorldMatrix, skinnedPos);

    //==========================================================
    // (4) ���� C++���� �Ѱ��� ��W * V * P�� �� �״�� ����ϱ� ����,
    //     ���⼭�� skinnedPos(= �� ����) ��� ��row-vector�� �������� ����
    //     ��, skinnedPos * (W * V * P) = ( ( (skinnedPos * W) * V ) * P )
    //
    //   * HLSL ������ mul(rowVector, matrix) ���·� ����
    //     ��rowVector �� matrix�� ������ �̷�����ϴ�.
    //==========================================================
    output.Position = mul(skinnedPos, WorldViewProjectionMatrix);

    //==========================================================
    // (5) ���� ��Ű�� + ���� ��ȯ
    //==========================================================
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

    //==========================================================
    // (6) �ȼ� ���̴��� ������ ������: ��� float4 ����
    //==========================================================
    output.Normal = float4(worldNormal3, 0.0f);
    output.TexCoord = float4(input.TexCoord.xy, 0.0f, 0.0f);
    output.Color = input.Color;

    return output;
}