#include "PrecompileHeader.h"
#include "Ext_FBXMeshComponent.h"
#include "Ext_MeshComponentUnit.h"
#include "Ext_FBXAnimator.h"

#include "Ext_DirectXVertexData.h"
#include <DirectX11_Base/Base_Directory.h>
#include "Ext_MeshLoader.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXMesh.h"
#include "Ext_DirectXTexture.h"
#include "Ext_Transform.h"
#include "Ext_MeshComponent.h"

#include "Ext_DirectXDevice.h"

Ext_FBXMeshComponent::Ext_FBXMeshComponent()
{
    Animator = std::make_shared<Ext_FBXAnimator>();
}

void Ext_FBXMeshComponent::Start()
{
    __super::Start();

    std::vector<Ext_DirectXVertexData> Vertices;
    std::vector<UINT> Indices;

    Base_Directory Dir;
    Dir.MakePath("../Resource/Mesh/Character/Mesh/Girl.fbx");

    Animator->LoadMeshFBX(Dir.GetPath(), Vertices, Indices);

    Ext_DirectXVertexBuffer::CreateVertexBuffer("Girl", Vertices);
    Ext_DirectXIndexBuffer::CreateIndexBuffer("Girl", Indices);
    Ext_DirectXMesh::CreateMesh("Girl");

    /////////////////////////////////////////////////////////////////////////

    Base_Directory Dir2;
    Dir2.MakePath("../Resource/Mesh/Character/Texture");
    std::vector<std::string> Paths = Dir2.GetAllFile({ "png", "tga", "dss" });
    for (const std::string& FilePath : Paths)
    {
        Dir2.SetPath(FilePath.c_str());
        std::string ExtensionName = Dir2.GetExtension();
        std::string FileName = Dir2.GetFileName();
        Ext_DirectXTexture::LoadTexture(FilePath.c_str(), FileName.c_str(), ExtensionName.c_str());
    }

    Base_Directory Dir3;
    Dir3.MakePath("../Resource/Mesh/Character/Animation/Idle.fbx");

    Animator->LoadAnimationFBX(Dir3.GetPath());
    Animator->SetAnimation(0); // 첫 번째 애니메이션 선택
}

// 메시 컴포넌트에 필요한 유닛 생성 및 저장
std::shared_ptr<Ext_MeshComponentUnit> Ext_FBXMeshComponent::CreateMeshComponentUnit(std::string_view _MeshName, std::string_view _MaterialName)
{
    auto NewUnit = __super::CreateMeshComponentUnit(_MeshName, _MaterialName);
    Unit = NewUnit;

    return NewUnit;
}

void Ext_FBXMeshComponent::Rendering(float _Deltatime, const float4x4& _ViewMatrix, const float4x4& _ProjectionMatrix)
{
    // 기본 렌더링 (Transform / Material 셋업 후)
    __super::Rendering(_Deltatime, _ViewMatrix, _ProjectionMatrix);

    AccumulatedAnimTime += _Deltatime;
    Animator->UpdateAnimation(AccumulatedAnimTime);
    Animator->RenderSkinnedMesh();
    Animator->CB = Animator->RenderSkinnedMesh();


    // ※ 여기서 GPU Map/Unmap 은 하지 않고, 단지 BufferSetter에 “어디에 얼마 크기 데이터를 복사할지” 만 지정
        // [3] 트랜스폼 상수버퍼 세팅하기
    //const TransformData& TFData = *(GetTransform()->GetTransformData().get());
    //Unit->BufferSetter.SetConstantBufferLink("TransformData", TFData);
    Unit->BufferSetter.SetConstantBufferLink(SkinnedCBName, &Animator->CB, sizeof(CB_SkinnedMatrix));

    // ▶ 4. 이후 Unit->Rendering()에서 실제 GPU에 복사(Map/Unmap) 및 드로우 콜이 수행된다.
    //    즉, Unit->Rendering() 안에서 BufferSetter.BufferSetting() 이 호출되어
    //    지금까지 등록된 모든 상수/샘플러/텍스처를 한 번에 GPU 파이프라인에 바인딩한다.
    //    그러므로, 여기선 더 이상 Map/Unmap 하지 않는다.
}
