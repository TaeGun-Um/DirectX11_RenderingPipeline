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

    // 1-1) T-pose FBX(메시 + 바인드 포즈) 로드
    std::vector<Ext_DirectXVertexData> vertices;
    std::vector<unsigned int> indices;
    Animator = std::make_shared<Ext_FBXAnimator>();

    Base_Directory Dir;
    Dir.MakePath("../Resource/Mesh/Character/Mesh/Girl.fbx");
    bool IsMeshLoad = Animator->LoadMeshFBX(Dir.GetPath(), vertices, indices);
    // → LoadMeshFBX 내부에서:
    //    • Assimp로 메시 + 바인드 포즈 스켈레톤 가져오기(aiProcess_ConvertToLeftHanded + FlipUVs 등)
    //    • ExtractBonesFromMesh() 로 BoneNameToInfo, BoneCount 세팅
    //    • vertices.push_back(...) & indices.push_back(...) 로 정점/인덱스 채우기
    if (!IsMeshLoad)
    { 
        MsgAssert("FBX 메시 로드 실패");
        return;
    }

    // 1-2) 이 정점/인덱스 데이터를 기반으로 GPU 버퍼( VertexBuffer / IndexBuffer ) 생성
    Ext_DirectXVertexBuffer::CreateVertexBuffer("Girl", vertices);
    Ext_DirectXIndexBuffer::CreateIndexBuffer("Girl", indices);
    Ext_DirectXMesh::CreateMesh("Girl");

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
    bool IsAnimLoad = Animator->LoadAnimationFBX(Dir3.GetPath());
    if (!IsAnimLoad)
    {
        MsgAssert("FBX 애니메이션 로드 실패");
        return;
    }
     
    Animator->SetAnimation(0); // 첫 번째 애니메이션 선택
}

// 메시 컴포넌트에 필요한 유닛 생성 및 저장
std::shared_ptr<Ext_MeshComponentUnit> Ext_FBXMeshComponent::CreateMeshComponentUnit(std::string_view _MeshName, std::string_view _MaterialName)
{
    auto NewUnit = __super::CreateMeshComponentUnit(_MeshName, _MaterialName);
    
    return NewUnit;
}

void Ext_FBXMeshComponent::Rendering(float _DeltaTime, const float4x4& _ViewMatrix, const float4x4& _ProjectionMatrix)
{
    // 기본 렌더링 (Transform / Material 셋업 후)
    __super::Rendering(_DeltaTime, _ViewMatrix, _ProjectionMatrix);

    AccumulatedAnimTime += _DeltaTime;
    Animator->UpdateAnimation(0.f);
    Animator->CB = Animator->RenderSkinnedMesh();
    Unit->BufferSetter.SetConstantBufferLink(SkinnedCBName, &Animator->CB, sizeof(CB_SkinnedMatrix));

}
