#pragma once
#include "Ext_ResourceManager.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"

// ������� ���ؽ����� ������ �����ϱ� ���� Ŭ����
class Ext_DirectXMesh : public Ext_ResourceManager<Ext_DirectXMesh>
{
	friend class Ext_MeshComponentUnit;

public:
	// constrcuter destructer
	Ext_DirectXMesh();
	~Ext_DirectXMesh();

	// delete Function
	Ext_DirectXMesh(const Ext_DirectXMesh& _Other) = delete;
	Ext_DirectXMesh(Ext_DirectXMesh&& _Other) noexcept = delete;
	Ext_DirectXMesh& operator=(const Ext_DirectXMesh& _Other) = delete;
	Ext_DirectXMesh& operator=(Ext_DirectXMesh&& _Other) noexcept = delete;

	// ���ؽ� ���ۿ� �ε��� ���� ���� �Է� �� �޽� ����
	static std::shared_ptr<Ext_DirectXMesh> CreateMesh(std::string_view _Name)
	{
		return CreateMesh(_Name, _Name, _Name);
	}

	// ���ؽ� ���ۿ� �ε��� ���� ���� �Է� �� ����
	static std::shared_ptr<Ext_DirectXMesh> CreateMesh(std::string_view _Name, std::string_view _VBName, std::string_view _IBName)
	{
		std::shared_ptr<Ext_DirectXMesh> NewMesh = Ext_ResourceManager::CreateNameResource(_Name);
		NewMesh->VertexBufferPtr = Ext_DirectXVertexBuffer::Find(_VBName);
		NewMesh->IndexBufferPtr = Ext_DirectXIndexBuffer::Find(_IBName);

		if ((nullptr == NewMesh->VertexBufferPtr) || (nullptr == NewMesh->IndexBufferPtr))
		{
			MsgAssert("�޽� ���� ����");
		}

		return NewMesh;
	}


	// Getter
	std::shared_ptr<class Ext_DirectXVertexBuffer> GetVertexBuffer() { return VertexBufferPtr; }
	std::shared_ptr<class Ext_DirectXIndexBuffer> GetIndexBuffer() { return IndexBufferPtr; }

protected:
	
private:
	D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	std::shared_ptr<class Ext_DirectXVertexBuffer> VertexBufferPtr; // ������� ������ �����
	std::shared_ptr<class Ext_DirectXIndexBuffer> IndexBufferPtr; // �ε������� ������ �����
	
	void MeshSetting(); // InputAssembler1(), InputAssembler2() ȣ��
	void InputAssembler1(); // ��ǲ����� 1�ܰ� �ǽ�, IASetVertexBuffers(), IASetPrimitiveTopology() �ǽ�
	void InputAssembler2(); // ��ǲ����� 2�ܰ� �ǽ�, IndexBufferSetting() ȣ��
};