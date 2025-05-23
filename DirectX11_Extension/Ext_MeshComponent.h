#pragma once
#include "Ext_Component.h"

// ������ ������Ʈ
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

	// �޽� ������Ʈ�� �ʿ��� ���� ����
	std::shared_ptr<class Ext_MeshComponentUnit> CreateMeshComponentUnit(std::string_view _Mesh, std::string_view _Material);

	// Getter, Setter
	std::weak_ptr<class Ext_Camera> GetOwnerCamera() { return OwnerCamera; }
	void SetOwnerCamera(std::shared_ptr<class Ext_Camera> _Camera) { OwnerCamera = _Camera; }

protected:
	virtual void Start() override;
	virtual void Update(float _DeltaTime) override {}
	virtual void Destroy() override {}

private:
	void PushMeshToCamera(std::string_view _CameraName); // �޽� ������Ʈ�� �����ϸ� ī�޶� �ֱ� ����(������ ���� ī�޶�� ��)
	void Rendering(float _Deltatime); // �޽� ������ ������Ʈ
	std::shared_ptr<class Ext_MeshComponentUnit> CreateMeshComponentUnit(); // �޽� ������Ʈ�� �ʿ��� ���� Units�� ���� 
	 
	std::weak_ptr<class Ext_Camera> OwnerCamera; // �ڽ��� ������ ī�޶�
	std::vector<std::shared_ptr<class Ext_MeshComponentUnit>> Units; // �ʿ� ���� ������
	
};