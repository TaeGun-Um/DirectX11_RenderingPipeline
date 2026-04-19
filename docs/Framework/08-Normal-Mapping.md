---
title: Normal Mapping 해보기
original_url: https://umtimos.tistory.com/203
original_date: 2025-06-07
migrated: 2026-04-19
category: Framework
---

# Normal Mapping

라이팅이 들어왔어도 평면 벽은 여전히 "그냥 평면"이다. **울퉁불퉁한 질감**은 실제 지오메트리를 잘게 쪼개서 만들면 너무 비싸고, 그래서 **픽셀마다 노말을 흉내 내는 Normal Mapping**이 표준 기법이 됐다.

## 아이디어

정점 노말은 표면 전체를 덮는 큼직한 방향. 여기에 **텍스처에 저장된 미세 노말**을 픽셀 단위로 덧입히면, 라이팅 계산이 마치 실제로 굴곡이 있는 것처럼 반응한다. 실제 메시는 평면인데 조명만 보면 굴곡이 있는 착시가 생긴다.

## 재료 준비

돌벽 같은 PBR 텍스처 세트에 보통 다음이 딸려 온다:
- **BaseColor** (Albedo)
- **Normal Map** — RGB 채널이 표면 법선 벡터를 인코딩
- Roughness / Metallic / AO 등 (이번 글에선 BaseColor·Normal만)

본 프로젝트는 [Fab](https://www.fab.com/)에서 받은 돌벽 텍스처를 예시로 쓴다.

## 셰이더 수정

### 텍스처 슬롯 추가
```hlsl
Texture2D BaseColorTex : register(t0);
Texture2D NormalTex    : register(t1);
SamplerState Sampler   : register(s0);
```

### Vertex Shader — TBN 정보 전달

노말 매핑의 핵심은 **Tangent-Space(탄젠트 공간)**. 텍스처에 저장된 노말은 이 공간 기준인데, 라이팅은 월드 공간에서 하니까 **TBN 행렬로 변환**해야 한다. 그 재료인 Tangent·Binormal 이 정점 속성에 추가된다.

```hlsl
struct VSInput
{
    float4 Position : POSITION;
    float4 TexCoord : TEXCOORD;
    float4 Normal   : NORMAL;
    float4 Tangent  : TANGENT;
    float4 Binormal : BINORMAL;
};

struct VSOutput
{
    float4 Position      : SV_POSITION;
    float2 TexCoord      : TEXCOORD0;
    float3 WorldPosition : TEXCOORD1;
    float3 WorldNormal   : NORMAL;
    float3 WorldTangent  : TANGENT;
    float3 WorldBinormal : BINORMAL;
};

VSOutput Static_VS(VSInput In)
{
    VSOutput Out;
    In.Position.w = 1.0f;

    Out.Position      = mul(In.Position, WorldViewProjectionMatrix);
    Out.WorldPosition = mul(In.Position, WorldMatrix).xyz;

    // 방향 벡터는 w=0으로 평행이동 무시
    Out.WorldNormal   = mul(float4(In.Normal.xyz,   0), WorldMatrix).xyz;
    Out.WorldTangent  = mul(float4(In.Tangent.xyz,  0), WorldMatrix).xyz;
    Out.WorldBinormal = mul(float4(In.Binormal.xyz, 0), WorldMatrix).xyz;

    Out.TexCoord = In.TexCoord.xy;
    return Out;
}
```

Assimp 로드 시 `aiProcess_CalcTangentSpace` 플래그가 켜져 있으면 Tangent·Binormal이 자동 계산된다 ([Assimp 로드 문서](05-Assimp-Mesh-Animation.md)).

### Pixel Shader — TBN 행렬 + 변환

```hlsl
float4 Graphics_PS(PSInput In) : SV_TARGET
{
    float4 Albedo = BaseColorTex.Sample(Sampler, In.TexCoord);

    // 1. 탄젠트 공간 노말 샘플 (-1..+1로 역매핑)
    float3 SampledNormalTS = NormalTex.Sample(Sampler, In.TexCoord).xyz * 2.0f - 1.0f;

    // 2. TBN 행렬 (행 기준 3x3)
    float3x3 TBN;
    TBN[0] = normalize(In.WorldTangent);
    TBN[1] = normalize(In.WorldBinormal);
    TBN[2] = normalize(In.WorldNormal);

    // 3. 탄젠트 공간 노말 → 월드 공간
    float3 MappedWorldNormal = normalize(mul(SampledNormalTS, TBN));

    // 4. 이후 기존 라이팅 계산에 MappedWorldNormal 사용
    // ...
}
```

핵심 포인트:
- **`* 2.0f - 1.0f`** — 텍스처는 `[0, 1]` 범위지만 노말은 `[-1, 1]`. 역매핑 필수.
- **TBN 행렬** — 탄젠트(U축), 바이노말(V축), 노말(표면법선)을 행으로 놓으면 탄젠트→월드 변환 행렬이 됨.

## 왜 탄젠트 공간을 쓰는가

대안으로 월드/객체 공간 노말 맵도 있지만:
- 월드 공간: 메시를 회전시키면 노말 맵 재작성 필요
- 객체 공간: 메시마다 노말 맵이 다름

**탄젠트 공간은 메시의 로컬 UV 방향을 기준**이므로 동일한 노말 맵을 회전·스케일·인스턴스 구분 없이 재사용 가능. 그래서 현대 파이프라인 기본.

## 얻은 결과

- 같은 폴리곤 수지만 **표면이 부조처럼 깊이감** 있게 보임.
- 빛 방향을 바꾸면 요철의 그림자 방향도 자연스럽게 따라 움직임.
- Fab 돌벽 텍스처 세트로 **드라마틱한 차이**를 체감. 평면이 진짜 돌벽 같아짐.

## 주의할 점 / 개선거리

- **TBN 직교화 (Gram-Schmidt)** — Tangent·Binormal이 완전히 수직이 아닐 수 있다. 필요 시 PS 안에서 직교화 보정
- **Normal Map 인코딩 변종** — DirectX 규약은 y-up, OpenGL 규약은 y-down. 혼용 시 y 반전 필요
- **디퍼드 파이프라인 호환** — 디퍼드에선 G-Buffer에 Tangent-space 노말을 별도 저장하는 게 일반적. 현재 구조는 PBR 머티리얼 쪽에서 활용 중
- **추가 맵들** — Roughness/Metallic/AO 맵 통합해 PBR로 확장
- **Parallax Mapping / Tessellation** — 노말 맵 한계를 넘어 진짜 굴곡감이 필요한 곳에 쓸 수 있는 차세대 기법

## 관련 현재 코드
- [Shader/FGraphics_PS.hlsl](../../Shader/FGraphics_PS.hlsl) — Forward 노말 맵
- [Shader/FStatic_VS.hlsl](../../Shader/FStatic_VS.hlsl) — TBN 전달 VS
- [Shader/PBRDeferredLight_PS.hlsl](../../Shader/PBRDeferredLight_PS.hlsl) — 디퍼드 + TBN
- [DirectX11_Extension/Ext_DirectXMesh.cpp](../../DirectX11_Extension/Ext_DirectXMesh.cpp) — Assimp로 Tangent·Binormal 로드
