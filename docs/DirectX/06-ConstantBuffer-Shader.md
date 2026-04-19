---
title: 상수버퍼와 셰이더
original_url: https://umtimos.tistory.com/183
original_date: 2025-05-26
migrated: 2026-04-19
category: DirectX
---

# 상수버퍼와 셰이더

삼각형·사각형을 띄우는 첫 예제에선 셰이더도 하나, 상수 버퍼도 없이 그냥 색만 돌려줬다. 하지만 프레임워크로 확장하려면 **수십 개의 셰이더**와 **프레임마다 바뀌는 WVP 행렬** 같은 걸 체계적으로 다뤄야 한다. 이 문서는 그 기반이 되는 두 가지 — 셰이더 컴파일 자동화와 상수 버퍼 — 를 다룬다.

## 셰이더 종류 정리

GPU에서 돌아가는 작은 프로그램들. HLSL로 작성하고 런타임에 컴파일한다.

| 종류 | 역할 |
|---|---|
| Vertex Shader | 정점 위치·속성 변환 |
| Pixel Shader | 픽셀별 색 계산 |
| Geometry Shader | 정점 기반 새 프리미티브 생성 |
| Compute Shader | 범용 병렬 계산 (GPGPU) |
| Hull / Domain (Tessellation) | 정점 세분화 |

본 프레임워크는 지금 VS/PS만 쓴다. 파티클·GPGPU 기능이 생기면 Compute 등도 추가 예정.

## 상수 버퍼 (Constant Buffer)

VB/IB와 같은 버퍼지만 용도가 다르다. VB/IB는 한 번 만들면 보통 안 바뀌지만, **상수 버퍼는 매 프레임 CPU에서 새 값을 써넣어 GPU로 보낸다**. 대표적인 게 WVP 행렬.

### 생성

```cpp
void Ext_DirectXConstantBuffer::CreateConstantBuffer(
    const D3D11_SHADER_BUFFER_DESC& _BufferDesc)
{
    ConstantBufferInfo.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    ConstantBufferInfo.ByteWidth      = _BufferDesc.Size;
    ConstantBufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // ★ CPU 쓰기 허용
    ConstantBufferInfo.Usage          = D3D11_USAGE_DYNAMIC;       // ★ Dynamic = Map 가능

    if (S_OK != Ext_DirectXDevice::GetDevice()->CreateBuffer(
            &ConstantBufferInfo, nullptr, ConstantBuffer.GetAddressOf()))
    {
        MsgAssert("상수 버퍼 생성 실패");
    }
}
```

`DYNAMIC + CPU_ACCESS_WRITE`가 매 프레임 갱신의 핵심 조합. 만약 `UpdateSubresource()`로 갱신하려 하면 내부 동기화 때문에 프레임별 갱신이 비효율적.

### Map / Unmap

```cpp
void Ext_DirectXConstantBuffer::ChangeData(const void* _Data, UINT _Size)
{
    if (ConstantBufferInfo.ByteWidth != _Size)
    {
        MsgAssert("상수버퍼와 데이터 크기가 다름");
        return;
    }

    D3D11_MAPPED_SUBRESOURCE Mapped = { 0 };
    Ext_DirectXDevice::GetContext()->Map(
        ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);

    memcpy_s(Mapped.pData, ConstantBufferInfo.ByteWidth, _Data, _Size);
    Ext_DirectXDevice::GetContext()->Unmap(ConstantBuffer, 0);
}
```

`WRITE_DISCARD`는 "기존 내용은 신경 쓰지 말고 새로 쓸게"라는 힌트. 드라이버가 락을 최소화하기 위해 **새 메모리 블록을 잡아 주는** 최적화 경로로 들어간다. 매 프레임 전체 갱신엔 이게 가장 빠르다.

## Vertex Shader 선언

```hlsl
cbuffer TransformData : register(b0)
{
    float4   LocalPosition;
    float4   LocalRotation;
    float4   LocalScale;
    float4   LocalQuaternion;
    float4x4 LocalMatrix;

    float4   WorldPosition;
    float4   WorldRotation;
    float4   WorldQuaternion;
    float4   WorldScale;
    float4x4 WorldMatrix;

    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float4x4 WorldViewMatrix;
    float4x4 WorldViewProjectionMatrix;
}

struct VSInput  { float4 Position : POSITION; float4 Color : COLOR;
                  float4 TexCoord : TEXCOORD; float4 Normal : NORMAL; };
struct VSOutput { float4 Position : SV_POSITION; float4 Color : COLOR;
                  float4 TexCoord : TEXCOORD;    float4 Normal : NORMAL; };

VSOutput Basic_VS(VSInput In)
{
    VSOutput Out;
    In.Position.w = 1.0f;
    Out.Position = mul(In.Position, WorldViewProjectionMatrix);
    Out.Color    = In.Color;
    Out.TexCoord = In.TexCoord;
    Out.Normal   = In.Normal;
    return Out;
}
```

### 바인딩 슬롯 종류

| 접두사 | 용도 | 개수 | 비고 |
|---|---|---|---|
| `b` | 상수 버퍼 | b0~b13 | 최대 4096 bytes |
| `t` | SRV(Texture 등) | t0~t127 | |
| `s` | Sampler | s0~s15 | |
| `u` | UAV (RW) | u0~u63 | Compute 전용 |

`b`와 `t`는 독립이라 `b0`와 `t0`를 동시에 써도 충돌 없음.

### 패킹 규칙 — 가장 주의할 부분

cbuffer는 **16byte 경계**에 맞춰 정렬된다. C++ 쪽에서 구조체를 그대로 memcpy로 밀어 넣으므로, 두 레이아웃이 바이트 단위로 일치해야 한다. 아래는 전형적인 함정:

```hlsl
cbuffer TransformData : register(b0)
{
    float4x4 WorldMatrix;  // 64 bytes
    float3   LightDir;     // 12 bytes
    float    Padding;      //  4 bytes  ← 이게 없으면 뒤에 이어붙는 값의 오프셋이 어긋남
};
```

C++ 구조체도 `alignas(16)` 붙이거나 명시적 패딩 필드를 두어 대응한다.

### VSInput
IA에서 넘긴 정점 속성. 순서만 맞으면 일부만 골라 받아도 됨.

### VSOutput
Rasterizer가 다음 단계로 흘려보낼 값. **`SV_POSITION` 같은 시스템 시맨틱 필수**. 나머지는 자유롭게 추가/제거.

## Pixel Shader 선언

```hlsl
struct PSInput { float4 Position : SV_POSITION; float4 Color : COLOR; };
float4 Basic_PS(PSInput In) : SV_TARGET { return In.Color; }
```

VSOutput의 시맨틱 순서와 이름을 맞춰야 보간값이 정상 전달된다.

## 셰이더 오토 컴파일 — 1단계: 파일 스캔

셰이더가 늘어날수록 하드코딩된 경로/엔트리포인트가 감당이 안 된다. `../Shader` 디렉토리를 스캔해 `.hlsl`을 전부 찾고, **파일명이 곧 엔트리포인트**라는 규칙으로 컴파일한다.

```cpp
void Ext_DirectXResourceLoader::ShaderCompile()
{
    Base_Directory Dir;
    Dir.MakePath("../Shader");
    std::vector<std::string> Paths = Dir.GetAllFile({ "hlsl" });

    for (const std::string& Path : Paths)
    {
        std::string Entry = Dir.FindEntryPoint(Path);
        Ext_DirectXShader::ShaderAutoCompile(Path, Entry.c_str());
    }
}
```

엔트리포인트 이름에 `_VS`·`_PS`·`_CS` 같은 접미사가 있으면 자동으로 종류를 판별:

```cpp
ShaderType Ext_DirectXShader::FindShaderType(std::string_view _Entry)
{
    if (_Entry.find("_VS") != npos) return ShaderType::Vertex;
    if (_Entry.find("_PS") != npos) return ShaderType::Pixel;
    // ... CS, GS
    return ShaderType::Unknown;
}
```

`D3DCompileFromFile`의 주요 파라미터:
- 파일 경로(wide string)
- `#include` 처리자 (`D3D_COMPILE_STANDARD_FILE_INCLUDE`)
- 엔트리포인트 이름
- 셰이더 프로파일 (`vs_5_0`, `ps_5_0`)
- 컴파일 플래그 (`PACK_MATRIX_ROW_MAJOR`, `SHADER_DEBUG`)
- 결과 blob / 에러 blob

## 셰이더 오토 컴파일 — 2단계: 리플렉션

여기가 실제로 "자동" 의 핵심이다. 컴파일된 바이트코드에 `D3DReflect()`를 태우면 HLSL 내부의 cbuffer/텍스처/샘플러 이름·슬롯을 **런타임에 알아낼 수 있다**. 이 정보로 C++ 쪽에서 대응되는 리소스를 자동 생성해 슬롯에 묶어준다.

```cpp
ID3D11ShaderReflection* Refl = nullptr;
D3DReflect(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(),
           IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&Refl));

D3D11_SHADER_DESC Info;
Refl->GetDesc(&Info);

for (UINT i = 0; i < Info.BoundResources; ++i)
{
    D3D11_SHADER_INPUT_BIND_DESC ResDesc;
    Refl->GetResourceBindingDesc(i, &ResDesc);

    switch (ResDesc.Type)
    {
    case D3D_SIT_CBUFFER:
    {
        auto CB = Refl->GetConstantBufferByName(ResDesc.Name);
        D3D11_SHADER_BUFFER_DESC BufferDesc;
        CB->GetDesc(&BufferDesc);

        auto MyCB = Ext_DirectXConstantBuffer::CreateConstantBuffer(
            ToUpper(ResDesc.Name), BufferDesc, BufferDesc.Size);

        ConstantBufferSetter Set;
        Set.OwnerShader    = GetSharedFromThis<Ext_DirectXShader>();
        Set.Name           = ToUpper(ResDesc.Name);
        Set.BindPoint      = ResDesc.BindPoint;
        Set.ConstantBuffer = MyCB;
        BufferSetter.InsertConstantBufferSetter(Set);
        break;
    }
    // D3D_SIT_TEXTURE, D3D_SIT_SAMPLER, D3D_SIT_STRUCTURED ...
    }
}

Refl->Release();   // ★ 중요
```

> **리플렉션 인터페이스는 반드시 `Release()` 호출** — ComPtr이 아닌 원시 포인터로 나오기 때문에 쉽게 누수된다. 최근 리팩토링에서 이 누수를 잡았다 ([WorkLog 2026-04-19](../WorkLog/2026-04-19.md)).

## 얻은 결과

- 셰이더 파일만 `../Shader/` 에 던져 놓으면 **자동으로 컴파일 + 슬롯 매핑**까지 완료됨. 수작업 코드가 사라진다.
- HLSL의 cbuffer 선언이 곧 C++ 쪽 `Ext_DirectXConstantBuffer` 등록과 연동됨. 크기 불일치는 런타임 assert로 잡힘.
- `Ext_DirectXBufferSetter` 가 "이름 → CPU 데이터 포인터 → GPU 버퍼"를 연결해, 사용자는 `SetConstantBufferLink("TransformData", data)` 한 줄이면 끝.

## 다음 단계 / 개선거리

- **HLSL ↔ C++ 패킹 검증 자동화** — `static_assert(sizeof(CppStruct) == ReflectedSize)` 훅. 지금은 런타임에서만 잡힌다.
- **Include 의존 처리** — `#include`로 cbuffer 공유할 때 엔트리포인트별 중복 생성 방지 (이름으로 캐싱은 되지만, 여러 파일에서 같은 `LightData` 를 재사용할 때 로그가 지저분해짐)
- **Shader Hot Reload** — 런타임에 파일 변경 감지해 재컴파일. 이터레이션 속도 크게 향상
- **Compute / Geometry 셰이더** — 파티클 시스템에 필요

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXConstantBuffer.{h,cpp}](../../DirectX11_Extension/Ext_DirectXConstantBuffer.h)
- [DirectX11_Extension/Ext_DirectXShader.{h,cpp}](../../DirectX11_Extension/Ext_DirectXShader.h) — `ShaderResourceSetting()`
- [DirectX11_Extension/Ext_DirectXBufferSetter.{h,cpp}](../../DirectX11_Extension/Ext_DirectXBufferSetter.h)
- [Shader/Transform.fx](../../Shader/Transform.fx) — TransformData cbuffer 정의
