﻿#pragma once
#include "Ext_Component.h"

// 렌더링을 위한 컴포넌트(static Mesh)
class Ext_MeshComponent : public Ext_Component
{
	friend class Ext_Camera;

public:
	// constrcuter destructer
	Ext_MeshComponent() {}
	~Ext_MeshComponent() {}

	// delete Function
	Ext_MeshComponent(const Ext_MeshComponent& _Other) = delete;
	Ext_MeshComponent(Ext_MeshComponent&& _Other) noexcept = delete;
	Ext_MeshComponent& operator=(const Ext_MeshComponent& _Other) = delete;
	Ext_MeshComponent& operator=(Ext_MeshComponent&& _Other) noexcept = delete;

	virtual std::shared_ptr<class Ext_MeshComponentUnit> CreateMeshComponentUnit(std::string_view _Mesh, MaterialType _SettingValue); // 메시 컴포넌트에 필요한 유닛 생성 및 저장
	virtual std::shared_ptr<class Ext_MeshComponentUnit> CreateMeshComponentUnit(std::string_view _MeshName, std::string_view _MaterialName);
	
	void SetTexture(std::string_view _TextureName, TextureType _TypeValue = TextureType::BaseColor); // 텍스쳐 변경하기
	void SetTexture(std::string_view _TextureName, std::string_view _SlotName);
	void SetSampler(SamplerType _TypeValue); // 샘플러 변경하기

	// Getter, Setter
	std::weak_ptr<class Ext_Camera> GetOwnerCamera() { return OwnerCamera; }
	void SetOwnerCamera(std::shared_ptr<class Ext_Camera> _Camera) { OwnerCamera = _Camera; }
	void ShadowOn(ShadowType _Type);
	void ShadowOff();

protected:
	virtual void Start() override;
	void Release() override;
	virtual void Rendering(float _Deltatime, const float4x4& _ViewMatrix, const float4x4& _ProjectionMatrix); // 메시 렌더링 업데이트

	void PushMeshToCamera(std::string_view _CameraName); // 메시 컴포넌트는 생성하면 카메라에 넣기 진행(지금은 메인 카메라로 들어감)
	
	std::weak_ptr<class Ext_Camera> OwnerCamera; // 자신을 소유한 카메라
	std::shared_ptr<class Ext_MeshComponentUnit> Unit; // Mesh와 Pipeline 정보 모음

private:
	
};