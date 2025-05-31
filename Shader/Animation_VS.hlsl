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
    float4 Position : POSITION; // 이미 float4
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL; // float4 입력
    uint4 BoneID : BONEID;
    float4 Weight : WEIGHT;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float4 Normal : NORMAL; //여전히 float4로 유지
};

VSOutput Animation_VS(VSInput input)
{
    VSOutput output;

    //==========================================================
    // (1) 스킨 행렬(skinMatrix) 계산: 본 배열(Bones[])과
    //     정점 BoneID/Weight를 가중치 합산(블렌딩)하여 구함
    //==========================================================
    float4x4 skinMatrix = float4x4(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    );

    // Bones[bIdx]는 이미 "글로벌 역행렬 × 애니메이션 적용 행렬 × OffsetMatrix" 형태로 계산됨
    // (Ext_FBXAnimator.RenderSkinnedMesh()에서 CB_SkinnedMatrix로 전달됨)
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
    // (2) Position 스키닝: 모델 바인드포즈 공간 정점(input.Position)
    //     을 skinMatrix로 변환
    //==========================================================
    // HLSL에서 행렬 인덱스 접근은 skinMatrix[row][col] 형태
    float4 skinnedPos;
    skinnedPos.x = dot(skinMatrix[0], input.Position); // 행(row) 0 × 정점
    skinnedPos.y = dot(skinMatrix[1], input.Position); // 행 1 × 정점
    skinnedPos.z = dot(skinMatrix[2], input.Position); // 행 2 × 정점
    skinnedPos.w = dot(skinMatrix[3], input.Position); // 행 3 × 정점

    //==========================================================
    // (3) 모델 공간 → 월드 공간 변환
    //==========================================================
    float4 worldPos = mul(WorldMatrix, skinnedPos);

    //==========================================================
    // (4) 기존 C++에서 넘겨준 “W * V * P” 를 그대로 사용하기 위해,
    //     여기서는 skinnedPos(= 모델 공간) 대신 “row-vector” 형식으로 곱셈
    //     즉, skinnedPos * (W * V * P) = ( ( (skinnedPos * W) * V ) * P )
    //
    //   * HLSL 에서는 mul(rowVector, matrix) 형태로 쓰면
    //     “rowVector × matrix” 연산이 이루어집니다.
    //==========================================================
    output.Position = mul(skinnedPos, WorldViewProjectionMatrix);

    //==========================================================
    // (5) 법선 스키닝 + 월드 변환
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
    // (6) 픽셀 셰이더로 전달할 데이터: 모두 float4 형식
    //==========================================================
    output.Normal = float4(worldNormal3, 0.0f);
    output.TexCoord = float4(input.TexCoord.xy, 0.0f, 0.0f);
    output.Color = input.Color;

    return output;
}