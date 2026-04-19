---
title: InputAssembler 단계 준비와 도형 출력
original_url: https://umtimos.tistory.com/178
original_date: 2025-05-21
migrated: 2026-04-19
category: DirectX
---

# InputAssembler 단계 준비와 도형 출력

여기서부터가 "진짜 그래픽스" 구간이다. 앞서 만든 Device·SwapChain·RenderTarget은 물감과 도화지를 준비한 거고, 이제 붓을 움직여 도형을 그릴 차례다. **InputAssembler(IA)** 는 파이프라인의 가장 앞 단계로, 정점 데이터를 어떻게 GPU에 공급할지 결정한다.

## 정점과 InputLayout

렌더링의 재료는 정점(Vertex)이다. 삼각형 하나를 그리려면 정점 3개, 사각형은 정점 4개. 각 정점은 위치만이 아니라 색·UV·노멀 같은 여러 속성을 가질 수 있는데, 그 **속성 구조를 GPU에게 알려주는 게 InputLayout**이다.

```cpp
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("COLOR",    DXGI_FORMAT_R32G32B32A32_FLOAT);
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("NORMAL",   DXGI_FORMAT_R32G32B32A32_FLOAT);
```

첫 인자는 **시맨틱 이름**(VS 입력과 정확히 일치해야 함), 둘째는 **데이터 형식**. `R32G32B32A32_FLOAT`는 `float4`와 같다.

내부 구현은 오프셋을 자동 누적해가며 `D3D11_INPUT_ELEMENT_DESC` 배열을 채운다:

```cpp
void InputLayoutData::AddInputLayoutDesc(
    LPCSTR _SemanticName, DXGI_FORMAT _Format,
    D3D11_INPUT_CLASSIFICATION _InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
    UINT _InstanceDataStepRate = 0, UINT _AlignedByteOffset = 0,
    UINT _InputSlot = 0, UINT _SemanticIndex = 0)
{
    D3D11_INPUT_ELEMENT_DESC Data;
    Data.SemanticName         = _SemanticName;
    Data.Format               = _Format;
    Data.InputSlotClass       = _InputSlotClass;
    Data.InstanceDataStepRate = _InstanceDataStepRate;
    Data.AlignedByteOffset    = Offset;   // 누적 오프셋 자동 계산
    Data.InputSlot            = _InputSlot;
    Data.SemanticIndex        = _SemanticIndex;

    Offset += FormatSize(Data.Format);
    InputLayoutDescs.push_back(Data);
}
```

### 필드별 의미

| 필드 | 설명 |
|---|---|
| `SemanticName` | VS 입력의 시맨틱과 매칭 |
| `Format` | 데이터 형식 — `R32G32B32A32_FLOAT` = `float4` |
| `InputSlotClass` | 정점당 (`PER_VERTEX_DATA`) / 인스턴스당 (`PER_INSTANCE_DATA`) |
| `InstanceDataStepRate` | 인스턴싱 시 몇 인스턴스마다 값이 바뀌는지 |
| `AlignedByteOffset` | 구조체 내 바이트 오프셋 |
| `InputSlot` | `IASetVertexBuffers` 슬롯 번호 |
| `SemanticIndex` | 같은 이름 여러 개일 때 구분 (`TEXCOORD0`, `TEXCOORD1`) |

## VertexBuffer — 정점 데이터 덩어리

```cpp
void Ext_DirectXVertexBuffer::CreateVertexBuffer(
    const void* _Data, UINT _VertexSize, UINT _VertexCount)
{
    VertexSize  = _VertexSize;
    VertexCount = _VertexCount;

    VertexBufferInfo.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferInfo.ByteWidth      = VertexSize * VertexCount;
    VertexBufferInfo.CPUAccessFlags = 0;
    VertexBufferInfo.Usage          = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA Init;
    Init.pSysMem = _Data;

    Ext_DirectXDevice::GetDevice()->CreateBuffer(
        &VertexBufferInfo, &Init, VertexBuffer.GetAddressOf());
}
```

고정 데이터(정점)는 `USAGE_DEFAULT` — GPU 전용 메모리에 올려 성능을 뽑는다. CPU에서 자주 바꿀 데이터면 `USAGE_DYNAMIC`을 써야 하지만, 정점은 보통 그럴 일이 없다.

## IndexBuffer — 정점 그리기 순서

같은 정점을 여러 번 참조할 수 있으므로 정점 재사용으로 메모리를 아낄 수 있다. 사각형을 두 삼각형으로 그릴 때 `{0, 1, 2, 0, 2, 3}`처럼 정점 4개를 6번 참조.

```cpp
void Ext_DirectXIndexBuffer::CreateIndexBuffer(
    const void* _Data, UINT _IndexSize, UINT _IndexCount)
{
    switch (_IndexSize)
    {
    case 2: Format = DXGI_FORMAT_R16_UINT; break;  // 65k 이하 정점용
    case 4: Format = DXGI_FORMAT_R32_UINT; break;  // 큰 메시
    }

    IndexBufferInfo.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferInfo.ByteWidth = _IndexSize * _IndexCount;
    IndexBufferInfo.Usage     = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA Init; Init.pSysMem = _Data;
    Ext_DirectXDevice::GetDevice()->CreateBuffer(
        &IndexBufferInfo, &Init, IndexBuffer.GetAddressOf());
}
```

## 실제로 도형 만들어보기

```cpp
// 삼각형
std::vector<Ext_DirectXVertexData> V(3);
V[0] = { { 0.0f,  0.5f, 0.0f }, { 1, 0, 0, 1 } };
V[1] = { { 0.5f, -0.5f, 0.0f }, { 0, 1, 0, 1 } };
V[2] = { {-0.5f, -0.5f, 0.0f }, { 0, 0, 1, 1 } };
std::vector<UINT> I = { 0, 1, 2 };

Ext_DirectXVertexBuffer::CreateVertexBuffer("Triangle", V);
Ext_DirectXIndexBuffer ::CreateIndexBuffer ("Triangle", I);

// 사각형 = 삼각형 두 개
std::vector<Ext_DirectXVertexData> Q(4);
Q[0] = { { 0.5f,  0.5f, -0.5f, 1.f}, {1, 0, 0, 1}, {1, 0}, {0, 0, -1} };
Q[1] = { {-0.5f,  0.5f, -0.5f, 1.f}, {0, 1, 0, 1}, {0, 0}, {0, 0, -1} };
Q[2] = { {-0.5f, -0.5f, -0.5f, 1.f}, {0, 0, 1, 1}, {0, 1}, {0, 0, -1} };
Q[3] = { { 0.5f, -0.5f, -0.5f, 1.f}, {1, 1, 0, 1}, {1, 1}, {0, 0, -1} };
std::vector<UINT> QI = { 0, 1, 2, 0, 2, 3 };
```

D3D의 기본 프리미티브는 삼각형이다. 사각형도 삼각형 두 개로 쪼개 그리는 게 표준.

## 셰이더 컴파일

```hlsl
// BaseVertexShader.hlsl
struct VSInput  { float3 Position : POSITION; float4 Color : COLOR; };
struct PSInput  { float4 Position : SV_POSITION; float4 Color : COLOR; };

PSInput main(VSInput In)
{
    PSInput Out;
    Out.Position = float4(In.Position, 1.0f);
    Out.Color    = In.Color;
    return Out;
}
```

```hlsl
// BasePixelShader.hlsl
struct PSInput { float4 Position : SV_POSITION; float4 Color : COLOR; };
float4 main(PSInput In) : SV_TARGET { return In.Color; }
```

```cpp
UINT Flag = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
Flag |= D3D10_SHADER_DEBUG;
#endif

COMPTR<ID3DBlob> VSBlob, ErrBlob;
D3DCompileFromFile(L"../Shader/BaseVertexShader.hlsl", nullptr,
    D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", Flag, 0,
    VSBlob.GetAddressOf(), ErrBlob.GetAddressOf());

Ext_DirectXDevice::GetDevice()->CreateVertexShader(
    VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(),
    nullptr, BaseVertexShader.GetAddressOf());
```

파일 경로는 wide string(`L"..."`)이 필요하다. 실패하면 `ErrBlob`에 에러 메시지가 실려 있으니 꺼내서 MessageBox에 띄우면 디버깅이 편하다.

## CreateInputLayout

VS 바이트코드가 있어야 InputLayout을 만들 수 있다. VS와 Layout이 맞는지 GPU가 이 단계에서 검증하기 때문.

```cpp
void Ext_DirectXInputLayout::CreateInputLayout(
    std::shared_ptr<Ext_DirectXVertexBuffer> _VB,
    std::shared_ptr<Ext_DirectXVertexShader> _VS)
{
    Release();
    const auto& Desc = _VB->GetInputLayout()->GetInputLayoutDescs();

    HRESULT hr = Ext_DirectXDevice::GetDevice()->CreateInputLayout(
        Desc.data(), (UINT)Desc.size(),
        _VS->GetBinaryCode()->GetBufferPointer(),
        _VS->GetBinaryCode()->GetBufferSize(),
        InputLayout.GetAddressOf());

    if (S_OK != hr) MsgAssert("CreateInputLayout 실패");
}
```

## 실제로 한 프레임 그려보기

```cpp
void Ext_Core::RenderTest()
{
    auto MainRT = Ext_DirectXDevice::GetMainRenderTarget();
    auto RTV    = MainRT->GetTexture(0)->GetRTV();
    auto DSV    = MainRT->GetDepthTexture()->GetDSV();
    auto VP     = MainRT->GetViewPort(0);
    auto* ctx   = Ext_DirectXDevice::GetContext();

    float Clear[4] = { 0.f, 0.f, 1.f, 1.f };
    ctx->ClearRenderTargetView(RTV.Get(), Clear);
    ctx->ClearDepthStencilView(DSV.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    ctx->OMSetRenderTargets(1, RTV.GetAddressOf(), DSV.Get());
    ctx->RSSetViewports(1, VP);

    auto VB = Ext_DirectXVertexBuffer::Find("Triangle");
    auto IB = Ext_DirectXIndexBuffer ::Find("Triangle");

    UINT stride = VB->GetVertexSize(), offset = 0;
    ctx->IASetVertexBuffers(0, 1, VB->GetVertexBuffer().GetAddressOf(), &stride, &offset);
    ctx->IASetIndexBuffer  (IB->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetInputLayout  (Ext_DirectXResourceLoader::GetInputLayout().Get());
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(Ext_DirectXResourceLoader::GetVertexShader(), nullptr, 0);
    ctx->PSSetShader(Ext_DirectXResourceLoader::GetPixelShader (), nullptr, 0);

    ctx->DrawIndexed(IB->GetVertexCount(), 0, 0);

    Ext_DirectXDevice::GetSwapChain()->Present(1, 0);
}
```

## 얻은 결과

- **파란 배경에 삼각형/사각형**이 뜬다. 드디어 뭔가 그린 것이 보인다.
- 정점별 색이 부드럽게 그라데이션 되는 모습도 확인할 수 있다 — 이건 Rasterizer가 정점 속성을 픽셀 단위로 선형 보간해주기 때문.
- IA → VS → RS → PS → OM 전체 파이프라인이 한 번이라도 돌아가는 최소 예제 완성.

## 예상치 못한 현상

### 1. 도형이 가로로 늘어진다
ViewPort가 16:9인데 정점은 NDC(정사각형) 기준이라 가로로 늘어난다. 이건 다음 단계 [카메라와 행렬](05-Camera-Matrix.md)에서 View/Projection 행렬로 해결한다.

### 2. 정점 색이 섞여 있다
Rasterizer가 정점 속성을 픽셀마다 선형 보간하기 때문. 의도된 동작. 나중에 UV/텍스처 기반으로 바꾸면 이 보간이 텍스처 샘플링의 기반이 된다.

## 다음 단계

- [카메라와 행렬](05-Camera-Matrix.md) — 늘어진 도형을 바로잡고, 3D 공간에 배치
- [상수버퍼와 셰이더](06-ConstantBuffer-Shader.md) — 변환 행렬을 셰이더에 넘기기
- 개선거리:
  - `D3D11_APPEND_ALIGNED_ELEMENT` 를 사용해 offset 자동 계산 옵션도 지원
  - 큰 메시에서 `R16_UINT` vs `R32_UINT` 선택 자동화
  - 정점 포맷을 다양화 (`half4`, `R8G8B8A8_UNORM` 등 포맷 최적화)

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXVertexBuffer.h](../../DirectX11_Extension/Ext_DirectXVertexBuffer.h)
- [DirectX11_Extension/Ext_DirectXIndexBuffer.h](../../DirectX11_Extension/Ext_DirectXIndexBuffer.h)
- [DirectX11_Extension/Ext_DirectXInputLayout.h](../../DirectX11_Extension/Ext_DirectXInputLayout.h)
- [DirectX11_Extension/Ext_DirectXVertexData.h](../../DirectX11_Extension/Ext_DirectXVertexData.h)
