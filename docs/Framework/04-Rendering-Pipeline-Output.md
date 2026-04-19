---
title: 렌더링 파이프라인 설정 결과물 출력
original_url: https://umtimos.tistory.com/185
original_date: 2025-05-26
migrated: 2026-04-19
category: Framework
---

# 렌더링 파이프라인 설정 결과물 출력

Scene + Actor + Component + Mesh + Material 구조가 다 갖춰졌으니 이제 실제로 **이 모든 게 한 프레임 안에 맞물려 도는 모습**을 확인할 차례다. 이 문서는 한 프레임이 어떻게 진행되는지 순서대로 따라간다.

## 시나리오 — 단순한 사각형 하나

```cpp
// Contents_Core.cpp
void Contents_Core::Start()
{
    Ext_Core::CreateScene<TestScene>("TestScene");
    Ext_Core::ChangeScene("TestScene");
}

// TestScene.cpp
void TestScene::Start()
{
    CreateActor<RectActor>("RectActor");
}

// RectActor.cpp
void RectActor::Start()
{
    GetTransform()->SetWorldPosition({ 0.f, 0.f, 100.0f });
    auto MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh", true);
    MeshComp->CreateMeshComponentUnit("Rect", "Basic");
}
```

`Contents_Core`에서 씬을 만들고, 씬이 Actor를 만들고, Actor가 MeshComp를 달고, MeshComp가 "Rect" 메시 + "Basic" 머티리얼로 Unit을 생성한다. Actor의 월드 위치는 카메라 앞 100 단위.

## 한 프레임이 진행되는 순서

대략 아래 흐름:

```
 ClearRenderTargetView()           ─ 백버퍼 색 초기화
 ClearDepthStencilView()           ─ Depth 초기화
 OMSetRenderTargets()              ─ 이 프레임이 그려질 RT 지정
 RSSetViewports()                  ─ NDC→픽셀 매핑
 CameraTransformUpdate()           ─ View/Projection 행렬 계산
 SetViewProjectionMatrix → 각 Mesh ─ cbuffer용 WVP 조립
 IASetInputLayout()
 IASetVertexBuffers()
 IASetPrimitiveTopology()
 IASetIndexBuffer()
 VSSetShader()
 PSSetShader()
 Map / Unmap                       ─ cbuffer CPU 데이터 밀어 넣기
 VSSetConstantBuffers()
 PSSetConstantBuffers()
 DrawIndexed()                     ─ 실제 드로우 콜
 Present()                         ─ 백버퍼 → 화면
```

이 순서가 고정되기 때문에, 프레임워크가 대신 해주면 사용자는 씬 구성만 하면 된다.

## 단계별 코드 요약

**RTV / DSV Clear**
```cpp
void Ext_DirectXRenderTarget::RenderTargetViewsClear()
{
    for (auto& tex : Textures)
    {
        for (size_t j = 0; j < tex->GetRTVSize(); j++)
        {
            Ext_DirectXDevice::GetContext()->ClearRenderTargetView(
                tex->GetRTV(j).Get(), Colors[i].Arr1D);
        }
    }
}

void Ext_DirectXRenderTarget::DepthStencilViewClear()
{
    auto DSV = DepthTexture->GetDSV();
    Ext_DirectXDevice::GetContext()->ClearDepthStencilView(
        DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
```

**RTV + DSV + Viewport 바인딩**
```cpp
void Ext_DirectXRenderTarget::RenderTargetSetting()
{
    auto* ctx = Ext_DirectXDevice::GetContext();
    ctx->OMSetRenderTargets((UINT)RTVs.size(), RTV.GetAddressOf(), DSV.Get());
    ctx->RSSetViewports    ((UINT)ViewPorts.size(), &ViewPorts[0]);
}
```

**카메라 행렬 준비**
```cpp
void Ext_Camera::Rendering(float _dt)
{
    for (auto& [k, list] : MeshComponents)
        for (auto& mc : list)
            if (mc->GetIsUpdate())
                mc->Rendering(_dt, GetViewMatrix(), GetProjectionMatrix());
}
```

**Unit 파이프라인 세팅**
```cpp
void Ext_MeshComponentUnit::RenderUnitSetting()
{
    InputLayout->InputLayoutSetting();
    Mesh->MeshSetting();          // IA 1 + IA 2
    Material->MaterialSetting();  // VS + PS + OM
    BufferSetter.BufferSetting(); // cbuffer Map → VS/PSSetConstantBuffers
}

void Ext_MeshComponentUnit::RenderUnitDraw()
{
    UINT IndexCount = Mesh->GetIndexBuffer()->GetVertexCount();
    Ext_DirectXDevice::GetContext()->DrawIndexed(IndexCount, 0, 0);
}
```

**Present**
```cpp
void Ext_DirectXDevice::RenderEnd()
{
    HRESULT hr = SwapChain->Present(0, 0);
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        MsgAssert("디바이스가 리셋/제거됨");
    }
}
```

## 처음 본 결과

![초기](https://blog.kakaocdn.net/dna/nmRWI/btsOc64tB2e/AAAAAAAAAAAAAAAAAAAAAKnoCzjRLAX1gJJ_y_NgoKs0hQCh4wzBNA0oEfxrwyaX/img.png?credential=yqXZFxpELC7KVnFOS48ylbz2pIh7yKj8&expires=1777561199&allow_ip=&allow_referer=&signature=6%2BIOasSFPwTwG8fDVKMh7jlWS0U%3D)

카메라 기본값과 Actor 기본값이 조금 어긋나서 화면 중앙에 작은 초록색 점으로만 보인다. 카메라 거리를 조정하니까 정상적인 사각형이 확인됨.

![조정 후](https://blog.kakaocdn.net/dna/qDWki/btsOciq2jjc/AAAAAAAAAAAAAAAAAAAAAALn8bLr5uD0Jr7R6e8Vrk76FAY7dQvVuskmNgG0v1iZ/img.png?credential=yqXZFxpELC7KVnFOS48ylbz2pIh7yKj8&expires=1777561199&allow_ip=&allow_referer=&signature=dQF5V13coCkXPlDyaoAruBIi9gc%3D)

## 확장 해보기

### 여러 오브젝트 배치
```cpp
void TestScene::Start()
{
    GetMainCamera()->GetTransform()->SetWorldPosition({ 0.f, 0.f, 0.f });

    auto R1 = CreateActor<RectActor>("RectActor");
    R1->GetTransform()->SetWorldPosition({ 0.f, 0.f, 10.f });
    R1->CreateComponent<Ext_MeshComponent>("BasicMesh", true)
      ->CreateMeshComponentUnit("Rect", "Basic");

    auto R2 = CreateActor<RectActor>("RectActor2");
    R2->GetTransform()->SetWorldPosition({ 10.f, 0.f, 10.f });
    R2->CreateComponent<Ext_MeshComponent>("BasicMesh", true)
      ->CreateMeshComponentUnit("Rect", "Basic");
}
```
같은 메시·머티리얼을 위치만 달리 해서 복제.

### 회전 애니메이션
```cpp
void RectActor::Update(float _dt)
{
    float Speed = 100.0f;
    GetTransform()->AddWorldRotation({ 0.f, Speed * _dt, 0.f });
}
```
델타타임 기반 회전. 프레임레이트와 독립.

### 큐브 메시
6면 × 4정점 = 24정점, 인덱스는 면마다 `{0, 1, 2, 2, 3, 0}` 패턴 반복:

```cpp
std::vector<Ext_DirectXVertexData> V = {
    /* Front, Back, Left, Right, Top, Bottom — 각 4개씩 */
};
std::vector<UINT> I;
for (int i = 0; i < 6; ++i)
{
    int B = i * 4;
    I.insert(I.end(), { B+0, B+1, B+2, B+2, B+3, B+0 });
}

Ext_DirectXVertexBuffer::CreateVertexBuffer("Box", V);
Ext_DirectXIndexBuffer ::CreateIndexBuffer ("Box", I);
Ext_DirectXMesh::CreateMesh("Box");
```

### 큐브 액터 (3축 회전)
```cpp
void CubeActor::Update(float _dt)
{
    float Speed = 100.0f;
    GetTransform()->AddWorldRotation({ Speed * _dt, 0.f, 0.f });
    GetTransform()->AddWorldRotation({ 0.f, Speed * _dt, 0.f });
    GetTransform()->AddWorldRotation({ 0.f, 0.f, Speed * _dt });
}
```

## 얻은 결과

- **엔진의 첫 완성 루프**. 사용자는 Scene + Actor + MeshComp만 선언하면 DirectX가 매 프레임 돌아감.
- 이후 모든 기능(라이팅, 그림자, 반투명, 포스트 프로세스 등)이 이 파이프라인 위에 **레이어로 쌓이는** 구조가 잡힘.
- 프레임레이트 독립적(Δt 기반) 애니메이션이 당연한 것이 됨.

## 다음 단계 / 개선거리

- **Z 정렬** — 현재 프레임워크는 알파 패스에서 거리 정렬을 하지만, 불투명 패스엔 정렬 없음. Early-Z를 살리려면 near→far 정렬 필요
- **Culling** — 프러스텀/오클루전 컬링 미구현. 오브젝트가 많아질수록 드로우콜 절약 효과 큼
- **Instancing** — 똑같은 메시 여러 번 그리는 경우(파티클, 풀 등) 인스턴싱으로 대폭 최적화
- **명령 버퍼화** — 현재는 곧바로 Device Context에 명령 발행. 향후 커맨드 리스트 분리해 병렬 생성도 가능

## 관련 현재 코드
- [DirectX11_Extension/Ext_Camera.cpp](../../DirectX11_Extension/Ext_Camera.cpp) `Rendering()`
- [DirectX11_Extension/Ext_MeshComponentUnit.cpp](../../DirectX11_Extension/Ext_MeshComponentUnit.cpp) `RenderUnitSetting` / `RenderUnitDraw`
- [DirectX11_Contents/CubeActor.cpp](../../DirectX11_Contents/CubeActor.cpp)
- [DirectX11_Contents/RenderScene.cpp](../../DirectX11_Contents/RenderScene.cpp)
