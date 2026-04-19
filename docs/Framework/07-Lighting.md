---
title: Lighting 추가하기 (Directional + Point + Phong)
original_url: https://umtimos.tistory.com/200
original_date: 2025-06-06
migrated: 2026-04-19
category: Framework
---

# Lighting 추가하기 — Directional + Point + Phong

앞서 Lambert Diffuse 만으로 간단히 불을 켜봤다. 이제 본격적으로 **여러 광원**과 **재질 표현**이 가능한 구조로 확장한다. 목표는 세 가지:

1. **여러 광원 지원** — 씬에 N개의 라이트를 등록해 모두 합산
2. **라이트 종류** — Directional + Point
3. **Phong 반사** — 하이라이트(Specular)까지

## Light 클래스와 데이터

`Ext_Light`는 Actor로 구현. 위치·회전을 Transform에서 그대로 쓰므로, 다른 액터처럼 배치·회전하면 광원이 움직인다.

```cpp
struct LightData
{
    float4 LightColor           = { 1.0f, 1.0f, 1.0f, 0.25f };
    //     RGB = 광원 색, w = Ambient 강도

    float4 LightWorldPosition;
    float4 LightForwardVector;
    float4 CameraWorldPosition;  // Phong Specular 계산용

    float  NearDistance;
    float  FarDistance;
    float  AttenuationValue;     // Point Light 감쇠 계수
    int    LightType;            // 0=Directional, 1=Point
    bool   bIsLightSet;
};
```

씬은 `LightDatas` 라는 하나의 cbuffer에 **최대 64개** 까지 담는다:

```hlsl
cbuffer LightDatas : register(b2)
{
    int LightCount;
    LightData Lights[64];
};
```

매 프레임 Scene Update에서 라이트들을 순회하며 `LightCount`를 초기화한 뒤 값 넣기:

```cpp
LightDataBuffer.LightCount = 0;
for (auto& [name, light] : Lights)
{
    light->LightUpdate(camera, dt);
    LightDataBuffer.Lights[LightDataBuffer.LightCount] = *light->GetLightData();
    ++LightDataBuffer.LightCount;
}
```

## Vertex Shader — 월드 공간으로 정보 전달

라이팅 계산은 픽셀 셰이더에서 하므로, VS는 필요한 월드 공간 값을 넘겨줘야 한다:

```hlsl
struct VSOutput
{
    float4 Position      : SV_POSITION;
    float4 TexCoord      : TEXCOORD0;
    float3 WorldPosition : TEXCOORD1;
    float3 WorldNormal   : NORMAL;
};

VSOutput Static_VS(VSInput In)
{
    VSOutput Out;
    In.Position.w = 1.0f;

    Out.Position      = mul(In.Position, WorldViewProjectionMatrix);
    Out.WorldPosition = mul(In.Position, WorldMatrix).xyz;
    Out.WorldNormal   = mul(float4(In.Normal.xyz, 0.0f), WorldMatrix).xyz; // w=0 방향

    Out.TexCoord = In.TexCoord;
    return Out;
}
```

## Pixel Shader — 삼색 누적 (Phong)

Phong은 세 가지 성분을 합산:

- **Diffuse** — 빛이 표면에 직접 닿는 양 (람베르트)
- **Specular** — 하이라이트, 시점 각도에 따라 반짝
- **Ambient** — 간접광 근사, 완전히 그늘진 곳도 새까매지지 않도록 상수 가산

```hlsl
float3 AccumLightColor = float3(0, 0, 0);

for (int i = 0; i < LightCount; ++i)
{
    LightData L = Lights[i];
    if (!L.bIsLightSet) continue;

    float3 PixelPos     = In.WorldPosition;
    float3 PixelNormal  = normalize(In.WorldNormal);
    float3 EyeDir       = normalize(L.CameraWorldPosition.xyz - PixelPos);
    float  Shininess    = 32.0f;

    if (L.LightType == 0) // Directional
    {
        float3 LightDir = normalize(-L.LightForwardVector.xyz);

        // Diffuse
        float Diff = saturate(dot(PixelNormal, LightDir));

        // Phong Specular
        float3 R     = normalize(2.0f * PixelNormal * dot(LightDir, PixelNormal) - LightDir);
        float  Spec  = pow(max(0.0f, dot(R, EyeDir)), Shininess);

        float3 Ambient = L.LightColor.w;    // w 채널 = Ambient 강도

        AccumLightColor += L.LightColor.xyz * (Diff + Spec + Ambient);
    }
    else if (L.LightType == 1) // Point
    {
        float3 LightPos       = L.LightWorldPosition.xyz;
        float3 PixelToLight   = LightPos - PixelPos;
        float  Distance       = length(PixelToLight);
        float3 LightDir       = normalize(PixelToLight);

        // Attenuation (거리 기반 감쇠)
        float C0 = 1.0f;
        float C1 = 0.0f;
        float C2 = L.AttenuationValue / (L.FarDistance * L.FarDistance);
        float Attenuation = 1.0f / (C0 + C1 * Distance + C2 * Distance * Distance);

        float  Diff = saturate(dot(PixelNormal, LightDir));
        float3 R    = normalize(2.0f * PixelNormal * dot(LightDir, PixelNormal) - LightDir);
        float  Spec = pow(max(0.0f, dot(R, EyeDir)), Shininess);

        AccumLightColor += L.LightColor.xyz * (Diff + Spec + L.LightColor.w) * Attenuation;
    }
}

Albedo.rgb *= AccumLightColor;
```

### Point Light 감쇠 공식

```
1 / (C0 + C1·d + C2·d²)
```
- `C0=1`로 광원 위치에서 1.0 유지
- `C2 = Attenuation / Far²` 로 Far 거리에서 감쇠율이 `Attenuation`에 수렴
- `C1(선형항)`은 0 — 게임에서는 선형/상수만 있으면 오버. 2차 항이 가장 자연스럽다

## 얻은 결과

- 씬에 Directional 한 개 + Point 몇 개 섞어도 자연스럽게 누적.
- 메시 표면이 **빛 방향에 따라 밝아지고 어두워짐**. 카메라가 움직이면 **하이라이트도 따라 움직임** (Phong 특성).
- PBR로 가기 전이지만, 이 정도만 해도 "불 켜진 것" 티가 확실히 남.

## 다음 단계 / 개선거리

- **Normal Mapping** — 평면 텍스처에 미세 굴곡 → [Normal Mapping 문서](08-Normal-Mapping.md)
- **Shadow Mapping** — 라이트에서 보이지 않는 픽셀에 그림자 드리우기
- **PBR (Physically Based Rendering)** — Phong은 쉽지만 에너지 보존이 깨져 과장돼 보임. Cook-Torrance / GGX 도입
- **Forward → Deferred 전환** — 현재는 Forward 경로로, 라이트 수 ∝ 픽셀 셰이더 복잡도. 디퍼드로 옮기면 N 라이트에도 픽셀당 비용 상수
- **그림자·Ambient 분리** — 지금은 Light의 w에 Ambient가 들어있음. 전역 Ambient로 분리 권장

## 관련 현재 코드
- [DirectX11_Extension/Ext_Light.{h,cpp}](../../DirectX11_Extension/Ext_Light.h)
- [Shader/LightData.fx](../../Shader/LightData.fx) — 공용 cbuffer·헬퍼
- [Shader/FGraphics_PS.hlsl](../../Shader/FGraphics_PS.hlsl) — Forward 라이트 PS
