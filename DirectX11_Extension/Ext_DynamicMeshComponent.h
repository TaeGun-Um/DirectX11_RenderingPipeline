#pragma once

#include "Ext_MeshComponent.h"

class Ext_DynamicMeshComponent : public Ext_MeshComponent
{
public:
    Ext_DynamicMeshComponent() {}
    ~Ext_DynamicMeshComponent() {}

    Ext_DynamicMeshComponent(const Ext_DynamicMeshComponent&) = delete;
    Ext_DynamicMeshComponent(Ext_DynamicMeshComponent&&) noexcept = delete;
    Ext_DynamicMeshComponent& operator=(const Ext_DynamicMeshComponent&) = delete;
    Ext_DynamicMeshComponent& operator=(Ext_DynamicMeshComponent&&) noexcept = delete;

    std::shared_ptr<class Ext_MeshComponentUnit> CreateMeshComponentUnit(std::string_view _Mesh, MaterialSetting _SettingValue) override; // �޽� ������Ʈ�� �ʿ��� ���� ���� �� ����
    void CreateAnimation(std::string_view _FilePath);
    void SetAnimation(std::string_view _AnimName, bool _IsLoop = false);
    bool IsAnimationEnd();

    std::shared_ptr<class Ext_Animator> GetAnimator() { return Animator; };

protected:
    void Start() override;
    void Rendering(float _DeltaTime, const float4x4& _ViewMatrix, const float4x4& _ProjectionMatrix) override;

private:
    float AccumulatedTime = 0.0f; // �ִϸ��̼� ��� �ð��� ������ ����
    std::string SkinnedCBName = "CB_SkinnedMatrix"; 
    std::shared_ptr<class Ext_Animator> Animator = nullptr; // 
};