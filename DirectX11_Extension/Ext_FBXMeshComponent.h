#pragma once

#include "Ext_MeshComponent.h"

class Ext_FBXMeshComponent : public Ext_MeshComponent
{
public:
    Ext_FBXMeshComponent();
    ~Ext_FBXMeshComponent() {}

    Ext_FBXMeshComponent(const Ext_FBXMeshComponent&) = delete;
    Ext_FBXMeshComponent(Ext_FBXMeshComponent&&) noexcept = delete;
    Ext_FBXMeshComponent& operator=(const Ext_FBXMeshComponent&) = delete;
    Ext_FBXMeshComponent& operator=(Ext_FBXMeshComponent&&) noexcept = delete;

    std::shared_ptr<class Ext_MeshComponentUnit> CreateMeshComponentUnit(std::string_view _Mesh, std::string_view _Material) override;

    std::shared_ptr<class Ext_FBXAnimator> GetAnimator() { return Animator; };

protected:
    void Start() override;
    void Rendering(float _DeltaTime, const float4x4& _ViewMatrix, const float4x4& _ProjectionMatrix) override;

private:
    std::shared_ptr<class Ext_FBXAnimator> Animator = nullptr;
    std::string SkinnedCBName = "CB_SkinnedMatrix"; 
    float AccumulatedAnimTime = 0.0f; // 애니메이션 재생 시간을 누적할 변수
    

};