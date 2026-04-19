---
title: Object마다의 렌더링 파이프라인 생성을 위한 구조 설정 (Mesh / Material / Unit)
original_url: https://umtimos.tistory.com/184
original_date: 2025-05-26
migrated: 2026-04-19
category: Framework
---

# Object마다의 렌더링 파이프라인 구조

지금까지는 셰이더·VB/IB·InputLayout 같은 걸 개별적으로 만들어 썼다. 이걸 매번 작성·바인딩하는 건 도형 몇 개 뛰워볼 때나 가능한 이야기. 수십 개 오브젝트가 있는 씬에선 "**이 오브젝트는 어떤 메시, 어떤 셰이더로 그린다**"를 **이름 한두 개로 선택**할 수 있어야 한다.

그래서 Object 단위 렌더링 파이프라인을 세 개의 개념으로 쪼갠다:

- **`Ext_DirectXMesh`** — 정점 + 인덱스 (IA 단계 정보)
- **`Ext_DirectXMaterial`** — VS / PS / BlendState / DepthState / Rasterizer (IA 이외 전부)
- **`Ext_MeshComponentUnit`** — 위 둘을 연결해 한 오브젝트의 **"한 번의 DrawCall"** 을 표현

사용 모양:

```cpp
void CubeActor::Start()
{
    GetTransform()->SetLocalScale({ 100.f, 100.f, 100.f });

    auto MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh", true);
    MeshComp->CreateMeshComponentUnit("Box", "Basic");   // Mesh 이름, Material 이름
}
```

이름 두 개만 넣으면 나머지는 알아서 엮인다.

## Ext_DirectXMesh

VB·IB와 Topology를 묶어 하나의 "메시"로 등록:

```cpp
class Ext_DirectXMesh : public Ext_ResourceManager<Ext_DirectXMesh>
{
public:
    // 이름 같은 VB/IB를 자동으로 찾아 묶음
    static std::shared_ptr<Ext_DirectXMesh> CreateMesh(std::string_view _Name)
    {
        return CreateMesh(_Name, _Name, _Name);
    }

    static std::shared_ptr<Ext_DirectXMesh> CreateMesh(
        std::string_view _Name,
        std::string_view _VBName,
        std::string_view _IBName)
    {
        auto NewMesh = Ext_ResourceManager::CreateNameResource(_Name);
        NewMesh->VertexBufferPtr = Ext_DirectXVertexBuffer::Find(_VBName);
        NewMesh->IndexBufferPtr  = Ext_DirectXIndexBuffer ::Find(_IBName);
        return NewMesh;
    }

    void MeshSetting()
    {
        VertexBufferPtr->VertexBufferSetting();
        Ext_DirectXDevice::GetContext()->IASetPrimitiveTopology(Topology);
        IndexBufferPtr->IndexBufferSetting();
    }

private:
    D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    std::shared_ptr<Ext_DirectXVertexBuffer> VertexBufferPtr;
    std::shared_ptr<Ext_DirectXIndexBuffer>  IndexBufferPtr;
};
```

리소스 로더에서:
```cpp
Ext_DirectXVertexBuffer::CreateVertexBuffer("Triangle", triVerts);
Ext_DirectXIndexBuffer::CreateIndexBuffer ("Triangle", triIdx);
Ext_DirectXMesh::CreateMesh("Triangle");     // 같은 이름으로 VB·IB 연결

Ext_DirectXVertexBuffer::CreateVertexBuffer("Rect", rectVerts);
Ext_DirectXIndexBuffer::CreateIndexBuffer ("Rect", rectIdx);
Ext_DirectXMesh::CreateMesh("Rect");
```

이렇게 해두면 `Ext_DirectXMesh::Find("Rect")` 로 언제든 꺼내 쓸 수 있다.

## Ext_DirectXMaterial

VS / PS / BlendState / DepthState / Rasterizer 등 **IA 이외의 모든 파이프라인 설정**을 묶는다:

```cpp
class Ext_DirectXMaterial : public Ext_ResourceManager<Ext_DirectXMaterial>
{
public:
    static std::shared_ptr<Ext_DirectXMaterial> CreateMaterial(std::string_view _Name)
    {
        return CreateNameResource(_Name);
    }

    void SetVertexShader (std::string_view _Name);
    void SetPixelShader  (std::string_view _Name);
    void SetBlendState   (std::string_view _Name);
    void SetDepthState   (std::string_view _Name);
    void SetRasterizer   (std::string_view _Name);

    void MaterialSetting()    // 파이프라인 전체 세팅
    {
        VertexShaderSetting();
        PixelShaderSetting();
        OutputMergerSetting();    // Blend + DepthStencil + Rasterizer
    }
};
```

리소스 로더에서 **머티리얼 = 파이프라인 프리셋** 을 미리 만들어 놓는다:

```cpp
// 단일 메시
auto Mat = Ext_DirectXMaterial::CreateMaterial("Basic");
Mat->SetVertexShader("Basic_VS");
Mat->SetPixelShader ("Basic_PS");
Mat->SetBlendState  ("BaseBlend");
Mat->SetDepthState  ("EngineDepth");
Mat->SetRasterizer  ("BasicRasterizer");

// PBR 용
auto MatPBR = Ext_DirectXMaterial::CreateMaterial("PBR");
MatPBR->SetVertexShader("Basic_VS");
MatPBR->SetPixelShader ("PBR_PS");
// ... 나머지 동일
```

현재 프로젝트엔 `Static`, `Dynamic`, `StaticAlpha`, `DeferredLight`, `DeferredMerge`, `Shadow` 등 수십 가지 프리셋이 등록돼 있다 ([Ext_DirectXResourceLoader.cpp](../../DirectX11_Extension/Ext_DirectXResourceLoader.cpp)).

## MeshComponentUnit — 둘을 잇는 접착제

하나의 DrawCall을 표현하는 단위. Mesh + Material + InputLayout + BufferSetter 조합을 가지고 있다:

```cpp
void Ext_MeshComponentUnit::MeshComponentUnitInitialize(
    std::string_view _MeshName, std::string_view _MaterialName)
{
    Mesh     = Ext_DirectXMesh::Find    (_MeshName);
    Material = Ext_DirectXMaterial::Find(_MaterialName);

    // 1. VB의 InputLayout + VS 바이트코드로 InputLayout 생성
    InputLayout->CreateInputLayout(Mesh->GetVertexBuffer(), Material->GetVertexShader());

    // 2. 리플렉션으로 이미 만들어진 상수버퍼 setter들을 복사해 옴
    BufferSetter.Copy(Material->GetVertexShader()->GetBufferSetter());
    BufferSetter.Copy(Material->GetPixelShader ()->GetBufferSetter());

    // 3. 공통 Transform cbuffer 링크
    const TransformData& Data = *OwnerMeshComponent.lock()->GetTransform()->GetTransformData();
    BufferSetter.SetConstantBufferLink("TransformData", Data);

    // 4. 카메라에 등록 (렌더링 대상 목록에 편입)
    GetOwnerMeshComponent().lock()->GetOwnerCamera().lock()
        ->PushMeshComponentUnit(GetSharedFromThis<Ext_MeshComponentUnit>(), RenderPath::Unknown);
}
```

이 한 번의 초기화로 "다음 프레임부터 내가 자동으로 렌더링된다"는 상태가 만들어진다.

### 계층 구조 시각화

```
MeshComponent
└── MeshComponentUnit (한 번의 DrawCall)
    ├── Ext_DirectXMesh
    │   ├── Ext_DirectXVertexBuffer
    │   └── Ext_DirectXIndexBuffer
    ├── Ext_DirectXMaterial
    │   ├── Ext_DirectXVertexShader
    │   ├── Ext_DirectXPixelShader
    │   ├── Ext_DirectXBlend
    │   ├── Ext_DirectXDepth
    │   └── Ext_DirectXRasterizer
    └── Ext_DirectXInputLayout  (VB와 VS가 있어야 생성 가능)
```

## 상수버퍼 링크 — 이름 기반 연결

리플렉션으로 수집한 cbuffer 정보에 CPU 쪽 데이터를 "이름으로" 꽂아 넣는다:

```cpp
void Ext_DirectXBufferSetter::SetConstantBufferLink(
    std::string_view _Name, const void* _Data, UINT _Size)
{
    auto range = ConstantBufferSetters.equal_range(ToUpper(_Name));
    for (auto it = range.first; it != range.second; ++it)
    {
        if (it->second.ConstantBuffer->GetBufferSize() != _Size)
        {
            MsgAssert("cbuffer 크기 불일치");
            return;
        }
        it->second.CPUData     = _Data;
        it->second.CPUDataSize = _Size;
    }
}
```

크기가 안 맞으면 즉시 assert. C++ 구조체 패킹을 셰이더와 맞춰야 한다는 경고가 런타임에 잡힌다.

## 얻은 결과

- "**이름 두 개** (Mesh/Material)만으로 오브젝트 생성 완료" — Actor 코드가 극단적으로 짧아진다.
- Mesh·Material·Shader 단위 **재사용성** 극대화. 같은 Box 메시 + 다른 머티리얼로 쉽게 분기.
- 리플렉션 기반 자동 바인딩 덕분에 새 셰이더 추가가 **셰이더 파일 하나 + 머티리얼 등록 한 번**이면 끝.
- `CubeActor`, `StaticMeshActor`, `ReflectionActor` 등 다양한 데모 액터들이 이 구조 위에서 동일한 패턴으로 돌아감.

## 다음 단계 / 개선거리

- **`Ext_DirectXConstantBuffer` 팩토리 버그** — 생성한 cbuffer를 캐시 맵에 insert 안 해서 매번 재생성되던 문제가 있었음. 최근 수정 ([WorkLog](../WorkLog/2026-04-19.md))
- **Material 상속/오버라이드** — 같은 프리셋을 베이스로 특정 파라미터만 바꾸는 기능 (에디터 친화적)
- **InputLayout 공용화** — 지금은 Unit마다 하나씩 만듦. VS 바이트코드가 같으면 공유 가능
- **Mesh 레벨 LOD** — `Ext_DirectXMesh`에 LOD 배열 추가해 거리별 선택

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXMesh.{h,cpp}](../../DirectX11_Extension/Ext_DirectXMesh.h)
- [DirectX11_Extension/Ext_DirectXMaterial.{h,cpp}](../../DirectX11_Extension/Ext_DirectXMaterial.h)
- [DirectX11_Extension/Ext_MeshComponentUnit.{h,cpp}](../../DirectX11_Extension/Ext_MeshComponentUnit.h)
- [DirectX11_Extension/Ext_DirectXBufferSetter.{h,cpp}](../../DirectX11_Extension/Ext_DirectXBufferSetter.h)
