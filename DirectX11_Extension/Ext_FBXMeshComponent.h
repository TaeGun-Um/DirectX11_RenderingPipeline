#pragma once

#include "Ext_MeshComponent.h"

class Ext_FBXMeshComponent : public Ext_MeshComponent
{
public:
    Ext_FBXMeshComponent();
    ~Ext_FBXMeshComponent() override;

    Ext_FBXMeshComponent(const Ext_FBXMeshComponent&) = delete;
    Ext_FBXMeshComponent(Ext_FBXMeshComponent&&) noexcept = delete;
    Ext_FBXMeshComponent& operator=(const Ext_FBXMeshComponent&) = delete;
    Ext_FBXMeshComponent& operator=(Ext_FBXMeshComponent&&) noexcept = delete;

    std::shared_ptr<class Ext_MeshComponentUnit> CreateMeshComponentUnit(std::string_view _Mesh, std::string_view _Material) override;

protected:
    void Rendering(float _Deltatime, const float4x4& _ViewMatrix, const float4x4& _ProjectionMatrix) override;

private:

};