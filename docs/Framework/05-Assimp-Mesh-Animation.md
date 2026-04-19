---
title: Assimp로 3D Mesh와 애니메이션 로드하기
original_url: https://umtimos.tistory.com/197
original_date: 2025-05-29
migrated: 2026-04-19
category: Framework
---

# Assimp로 3D Mesh와 애니메이션 로드하기

Cube·Sphere 같은 기본 도형만 그려보면 금방 한계가 온다. 실제 게임 캐릭터처럼 **외부에서 만든 3D 메시**를 불러오고, 나아가 **본(Bone) 기반 애니메이션**까지 돌리려면 전용 라이브러리가 필요하다. 이 프로젝트는 업계 표준 격인 **Assimp (Open Asset Import Library)**를 쓴다.

## Assimp 준비

[Assimp GitHub](https://github.com/assimp/assimp)에서 소스를 받아 CMake로 빌드한다. 본 프로젝트는 빌드된 파일을 `ThirdParty/Assimp/lib.zip`으로 동봉해 뒀고, `Unzip.bat` 한 번이면 배치된다.

구조:
```
ThirdParty/Assimp/
├── inc/assimp/        (헤더)
└── lib/x64/
    ├── Debug/
    │   ├── assimp-vc143-mtd.lib
    │   └── assimp-vc143-mtd.dll
    └── Release/
        └── ...
```

DLL은 exe와 같은 폴더에 복사되어야 하므로 post-build step 설정:
```
xcopy /Y /D "$(ProjectDir)..\ThirdParty\Assimp\lib\x64\Debug\assimp-vc143-mtd.dll" "$(OutDir)"
```

> Assimp는 Debug 빌드와 Release 빌드가 바이너리 호환이 안 된다. 각 Configuration에 맞는 `.lib`/`.dll`을 정확히 쓰는 게 필수.

## 정점 레이아웃 확장

뼈(Bone)가 붙은 메시는 정점마다 **4개의 뼈 인덱스**와 **그에 해당하는 가중치(Weight)** 를 추가로 저장해야 한다 (스키닝 기본):

```cpp
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("NORMAL",   DXGI_FORMAT_R32G32B32A32_FLOAT);
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("TANGENT",  DXGI_FORMAT_R32G32B32A32_FLOAT);
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("BINORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("BONEID",   DXGI_FORMAT_R32G32B32A32_SINT);   // ← 뼈 인덱스
Ext_DirectXVertexData::GetInputLayoutData()
    .AddInputLayoutDesc("WEIGHT",   DXGI_FORMAT_R32G32B32A32_FLOAT);  // ← 뼈 가중치
```

정적 메시와 다이나믹 메시는 입력 레이아웃이 달라, 본 프로젝트는 `Ext_MeshComponent` (정적) / `Ext_DynamicMeshComponent` (애니메이션)로 분리했다.

## 메시 로드

Assimp의 `Importer`로 파일을 읽어 씬 그래프를 얻는다:

```cpp
Assimp::Importer Importer;
const aiScene* Scene = Importer.ReadFile(Path,
    aiProcess_Triangulate          // 폴리곤을 전부 삼각형으로 분해
    | aiProcess_CalcTangentSpace   // Tangent·Binormal 자동 계산
    | aiProcess_LimitBoneWeights   // 정점당 뼈 개수를 4로 제한 (4×float4 슬롯에 맞춤)
    | aiProcess_GenNormals);       // 노말이 없으면 생성

if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
{
    MsgAssert("Assimp 로드 실패: " + std::string(Importer.GetErrorString()));
    return;
}
```

노드 계층을 재귀로 순회하며 `aiMesh`에서 정점/인덱스/뼈를 추출한다. 각 `aiBone`은 영향 받는 정점 목록과 가중치를 들고 있으므로 이를 정점 구조에 거꾸로 채워 넣어야 한다.

## 애니메이션 로드

애니메이션은 대개 메시 파일과 별도(`Idle.fbx`, `Walk.fbx` 식)로 관리된다. 뼈 이름 → 애니메이션 채널 매핑을 만들어 두면 재생 시 빠르다:

```cpp
std::unordered_map<std::string, const aiNodeAnim*> BoneNameToAnimChannel;
for (unsigned int a = 0; a < Animation->mNumChannels; ++a)
{
    const aiNodeAnim* Channel = Animation->mChannels[a];
    BoneNameToAnimChannel[Channel->mNodeName.C_Str()] = Channel;
}
```

**Root motion** 처리: 캐릭터가 애니메이션에 따라 실제 이동해야 할지(root motion on) 제자리만 할지(off) 결정. 보통 루트 본 이름(`Hips` 같은)을 기준으로 분기한다.

## 뼈 행렬 계산

매 프레임 애니메이션 시각에 맞춰 각 뼈의 최종 행렬을 갱신:

```cpp
void ReadNodeHierarchy(float _AccumulatedTime,
                       const aiNode* _CurNode,
                       const float4x4& _ParentTransform)
{
    std::string NodeName = _CurNode->mName.C_Str();

    // 이 노드에 해당하는 애니메이션 채널이 있으면 보간
    float4x4 NodeLocal;
    auto it = BoneNameToAnimChannel.find(NodeName);
    if (it != BoneNameToAnimChannel.end())
    {
        aiVector3D   Pos   = InterpolatePosition(_AccumulatedTime, it->second);
        aiQuaternion Rot   = InterpolateRotation(_AccumulatedTime, it->second);
        aiVector3D   Scale = InterpolateScale   (_AccumulatedTime, it->second);
        NodeLocal = ComposeTRS(Pos, Rot, Scale);
    }
    else
    {
        NodeLocal = Convert(_CurNode->mTransformation);
    }

    float4x4 Global = _ParentTransform * NodeLocal;

    // 뼈라면 skinning 행렬 채우기
    auto BoneIt = BoneNameToIndex.find(NodeName);
    if (BoneIt != BoneNameToIndex.end())
    {
        int Index = BoneIt->second;
        FinalBoneMatrices[Index] = GlobalInverseTransform * Global * BoneOffsetMatrices[Index];
    }

    for (unsigned i = 0; i < _CurNode->mNumChildren; ++i)
        ReadNodeHierarchy(_AccumulatedTime, _CurNode->mChildren[i], Global);
}
```

`OffsetMatrix`는 뼈의 **바인드 포즈(T-Pose) 역행렬** — 정점을 뼈 공간으로 옮기는 변환이다.

## GPU 스키닝 (HLSL)

뼈 행렬들을 상수 버퍼로 올리고, VS에서 가중치 합으로 블렌딩:

```hlsl
cbuffer BonesData : register(b3)
{
    float4x4 Bones[128];   // 뼈 행렬 배열
};

float4 Dynamic_VS(VSInput In) : SV_POSITION
{
    float4x4 SkinMatrix = (float4x4)0;
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        SkinMatrix += Bones[In.BoneID[i]] * In.Weight[i];
    }

    float4 SkinPos    = mul(In.Position, SkinMatrix);
    float3 SkinNormal = mul(float4(In.Normal.xyz, 0), SkinMatrix).xyz; // w=0 for normals
    // ... WorldViewProjectionMatrix 곱해서 Output 만들기
}
```

가중치가 잘 정규화돼 있다면 (`sum(Weight) == 1.0`) SkinMatrix는 뼈들의 가중 평균이 되어 자연스러운 변형이 나온다.

## 얻은 결과

- Assimp가 `.fbx`, `.obj`, `.dae` 등 **50개 이상의 포맷**을 지원하므로 Mixamo·Blender 어디서 받아오든 바로 쓴다.
- Mixamo에서 받은 **Michelle 캐릭터**와 Idle/Walk/Attack 등 여러 애니메이션이 하나의 `Character` 액터에서 전환 가능.
- 스키닝 결과가 GPU에서 돌아가므로 정점 수가 많아도 부담이 작음.
- 이후 **FSM과 연결**해 상태별 애니메이션 재생 구조가 만들어짐 (다음 단계 문서에서).

## 주의할 점 / 개선거리

- **Debug DLL 배포 함정** — `Unzip.bat`을 안 돌리고 Debug 빌드 하면 post-build xcopy가 실패해 빌드 실패 에러가 나는데, 환경 세팅 문제라 코드와는 무관
- **정점당 뼈 4개 제한** — `LimitBoneWeights`로 4로 고정. 얼굴 같은 복잡한 메시에선 부족할 수 있음 (8 슬롯 확장 또는 vertex skinning 기법 변경)
- **애니메이션 블렌딩** — 현재는 단일 애니메이션 재생만. Idle→Walk 전환 시 **블렌드 트리/크로스페이드** 미구현
- **루트 모션** — 현재는 제자리 애니메이션 가정. 루트 본 분리해서 캐릭터 이동에 반영하는 처리 필요

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXMesh.cpp](../../DirectX11_Extension/Ext_DirectXMesh.cpp) — Assimp 로드
- [DirectX11_Extension/Ext_DynamicMeshComponent.{h,cpp}](../../DirectX11_Extension/Ext_DynamicMeshComponent.h) — 애니메이션 메시 컴포넌트
- [DirectX11_Extension/Ext_Animator.{h,cpp}](../../DirectX11_Extension/Ext_Animator.h)
- [Shader/Dynamic_VS.hlsl](../../Shader/Dynamic_VS.hlsl) — 스키닝 VS
