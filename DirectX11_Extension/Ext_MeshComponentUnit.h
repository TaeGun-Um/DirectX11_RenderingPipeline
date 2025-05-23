#pragma once
#include "Ext_DirectXBufferSetter.h"

// ������ ������Ʈ�� �ʿ��� �������� ������ �ִ� Ŭ����
class Ext_MeshComponentUnit : public std::enable_shared_from_this<Ext_MeshComponentUnit>
{
	friend class Ext_Camera;
public:
	// constrcuter destructer
	Ext_MeshComponentUnit();
	~Ext_MeshComponentUnit();

	// delete Function
	Ext_MeshComponentUnit(const Ext_MeshComponentUnit& _Other) = delete;
	Ext_MeshComponentUnit(Ext_MeshComponentUnit&& _Other) noexcept = delete;
	Ext_MeshComponentUnit& operator=(const Ext_MeshComponentUnit& _Other) = delete;
	Ext_MeshComponentUnit& operator=(Ext_MeshComponentUnit&& _Other) noexcept = delete;

	void MeshComponentUnitInitialize(std::string_view _MeshName, std::string_view _MaterialName); // Mesh, Material Initialize
	

	// this ��������
	template<typename Type>
	std::shared_ptr<Type> GetSharedFromThis()
	{
		return std::dynamic_pointer_cast<Type>(shared_from_this());
	}

	// Getter, Setter
	std::weak_ptr<class Ext_MeshComponent> GetOwnerMeshComponent() { return OwnerMeshComponent; }
	void SetOwnerMeshComponent(std::weak_ptr<class Ext_MeshComponent> _OwnerMeshComponent) { OwnerMeshComponent = _OwnerMeshComponent; }
	std::shared_ptr<class Ext_DirectXMesh> GetMesh() { return Mesh; }
	std::shared_ptr<class Ext_DirectXMaterial> GetMaterial() { return Material; }

protected:
	
private:
	void Rendering(float _Deltatime);
	void RenderUnitSetting(); // Mesh, Material Setting
	void RenderUnitDraw(); // DrawIndexed Call

	std::weak_ptr<class Ext_MeshComponent> OwnerMeshComponent; // �ڽ��� ������ ī�޶�
	std::shared_ptr<class Ext_DirectXInputLayout> InputLayout = nullptr;
	//COMPTR<ID3D11InputLayout> InputLayOut = nullptr;
	std::shared_ptr<class Ext_DirectXMesh> Mesh = nullptr;
	std::shared_ptr<class Ext_DirectXMaterial> Material = nullptr;

	Ext_DirectXBufferSetter BufferSetter;
};