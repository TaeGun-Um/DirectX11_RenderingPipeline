#include "PrecompileHeader.h"
#include "Ext_DynamicMeshComponent.h"
#include "Ext_MeshComponentUnit.h"
#include "Ext_Animator.h"

#include "Ext_DirectXVertexData.h"
#include <DirectX11_Base/Base_Directory.h>
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXMesh.h"
#include "Ext_DirectXTexture.h"
#include "Ext_Transform.h"
#include "Ext_MeshComponent.h"

#include "Ext_DirectXDevice.h"

void Ext_DynamicMeshComponent::Start()
{
    __super::Start();   
}

// 메시 컴포넌트에 필요한 유닛 생성 및 저장, DynamicMesh는 Animator 생성
std::shared_ptr<Ext_MeshComponentUnit> Ext_DynamicMeshComponent::CreateMeshComponentUnit(std::string_view _MeshName, MaterialType _SettingValue)
{
    std::shared_ptr<Ext_MeshComponentUnit> NewUnit = __super::CreateMeshComponentUnit(_MeshName, _SettingValue);
    Animator = std::make_shared<Ext_Animator>();
    Animator->SetSkeltalMesh(_MeshName);
    Unit->GetBufferSetter().SetConstantBufferLink(SkinnedCBName, &Animator->GetCBMat(), sizeof(CB_SkinnedMatrix));

    return NewUnit;
}

void Ext_DynamicMeshComponent::CreateAnimation(std::string_view _FilePath)
{
    Animator->LoadAnimation(_FilePath);
}

void Ext_DynamicMeshComponent::SetAnimation(std::string_view _AnimName, bool _IsLoop /*= false*/)
{
    Animator->SetAnimation(_AnimName, _IsLoop);
}

bool Ext_DynamicMeshComponent::IsAnimationEnd()
{
    return Animator->IsAnimationEnd();
}

void Ext_DynamicMeshComponent::Rendering(float _DeltaTime, const float4x4& _ViewMatrix, const float4x4& _ProjectionMatrix)
{
    // 기본 렌더링 (Transform / Material 셋업 후)
    __super::Rendering(_DeltaTime, _ViewMatrix, _ProjectionMatrix);

    Animator->UpdateAnimation(_DeltaTime);
}
