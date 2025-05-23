#pragma once
#include "Ext_Component.h"

// 렌더링 컴포넌트
class Ext_MeshComponent : public Ext_Component
{
public:
	// constrcuter destructer
	Ext_MeshComponent();
	~Ext_MeshComponent();

	// delete Function
	Ext_MeshComponent(const Ext_MeshComponent& _Other) = delete;
	Ext_MeshComponent(Ext_MeshComponent&& _Other) noexcept = delete;
	Ext_MeshComponent& operator=(const Ext_MeshComponent& _Other) = delete;
	Ext_MeshComponent& operator=(Ext_MeshComponent&& _Other) noexcept = delete;

	// 메시 컴포넌트에 필요한 유닛 생성
	std::shared_ptr<class Ext_MeshComponentUnit> CreateMeshComponentUnit(std::string_view _Mesh, std::string_view _Material);

	// Getter, Setter
	std::weak_ptr<class Ext_Camera> GetOwnerCamera() { return OwnerCamera; }
	void SetOwnerCamera(std::shared_ptr<class Ext_Camera> _Camera) { OwnerCamera = _Camera; }

protected:
	virtual void Start() override;
	virtual void Update(float _DeltaTime) override {}
	virtual void Destroy() override {}

private:
	void PushMeshToCamera(std::string_view _CameraName); // 메시 컴포넌트는 생성하면 카메라에 넣기 진행(지금은 메인 카메라로 들어감)
	void Rendering(float _Deltatime); // 메시 렌더링 업데이트
	std::shared_ptr<class Ext_MeshComponentUnit> CreateMeshComponentUnit(); // 메시 컴포넌트에 필요한 유닛 Units에 저장 
	 
	std::weak_ptr<class Ext_Camera> OwnerCamera; // 자신을 소유한 카메라
	std::vector<std::shared_ptr<class Ext_MeshComponentUnit>> Units; // 필요 정보 모음집
	
};