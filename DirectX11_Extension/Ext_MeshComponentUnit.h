#pragma once
#include "Ext_DirectXBufferSetter.h"

// ������ ������Ʈ�� �ʿ��� ����(�޽�, Pipeline, ������� ���� ��)���� ������ �ִ� Ŭ����
class Ext_MeshComponentUnit : public std::enable_shared_from_this<Ext_MeshComponentUnit>
{
	friend class Ext_Camera;
	friend class Ext_MeshComponent;

public:
	// constrcuter destructer
	Ext_MeshComponentUnit();
	~Ext_MeshComponentUnit();

	// delete Function
	Ext_MeshComponentUnit(const Ext_MeshComponentUnit& _Other) = delete;
	Ext_MeshComponentUnit(Ext_MeshComponentUnit&& _Other) noexcept = delete;
	Ext_MeshComponentUnit& operator=(const Ext_MeshComponentUnit& _Other) = delete;
	Ext_MeshComponentUnit& operator=(Ext_MeshComponentUnit&& _Other) noexcept = delete;

	void MeshComponentUnitInitialize(std::string_view _MeshName, std::string_view _MaterialName); // �޽� ������Ʈ ���� ���� �� ȣ��

	// this ��������
	template<typename Type>
	std::shared_ptr<Type> GetSharedFromThis()
	{
		return std::dynamic_pointer_cast<Type>(shared_from_this());
	}

	// Getter, Setter
	std::shared_ptr<class Ext_DirectXMesh> GetMesh() { return Mesh; }
	std::shared_ptr<class Ext_DirectXMaterial> GetMaterial() { return Material; }
	std::weak_ptr<class Ext_MeshComponent> GetOwnerMeshComponent() { return OwnerMeshComponent; }

protected:
	
private:
	void SetOwnerMeshComponent(std::weak_ptr<class Ext_MeshComponent> _OwnerMeshComponent) { OwnerMeshComponent = _OwnerMeshComponent; }
	void Rendering(float _Deltatime); // RenderUnitSetting, RenderUnitDraw ȣ��
	void RenderUnitSetting(); // Mesh, Material Setting
	void RenderUnitDraw(); // ���� ������� ���̴��� ���� �޽� Draw �ǽ�(DrawIndexed Call)

	std::weak_ptr<class Ext_MeshComponent> OwnerMeshComponent;		// �ڽ��� ������ ī�޶�
	std::shared_ptr<class Ext_DirectXInputLayout> InputLayout = nullptr;	// Vertex Buffer�� Vertex Shader�� ������ ��ǲ ���̾ƿ� ���� �����
	std::shared_ptr<class Ext_DirectXMesh> Mesh = nullptr;						// ����, �ε��� ���� �����
	std::shared_ptr<class Ext_DirectXMaterial> Material = nullptr;				// ���������� ���� �����(���̴�, �����Ͷ����� ��)
	Ext_DirectXBufferSetter BufferSetter;													// ������� �����(�������, �ؽ�ó, ���� ��)
};