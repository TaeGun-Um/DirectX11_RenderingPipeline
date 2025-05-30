#include "PrecompileHeader.h"
#include "Ext_FBXMeshComponent.h"
#include "Ext_MeshComponentUnit.h"

Ext_FBXMeshComponent::Ext_FBXMeshComponent()
{

}

Ext_FBXMeshComponent::~Ext_FBXMeshComponent()
{
}

std::shared_ptr<Ext_MeshComponentUnit> Ext_FBXMeshComponent::CreateMeshComponentUnit(std::string_view _Mesh, std::string_view _Material)
{
    std::shared_ptr<Ext_MeshComponentUnit> NewUnit = __super::CreateMeshComponentUnit(_Mesh, _Material);

    // Ãß°¡

    return NewUnit;
}

void Ext_FBXMeshComponent::Rendering(float _Deltatime, const float4x4& _ViewMatrix, const float4x4& _ProjectionMatrix)
{
    __super::Rendering(_Deltatime, _ViewMatrix, _ProjectionMatrix);

}