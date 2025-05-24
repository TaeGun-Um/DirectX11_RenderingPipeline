#pragma once
#include "Ext_ResourceManager.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"

// 만들어진 버텍스들의 정보를 저장하기 위한 클래스
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

	// 버텍스 버퍼와 인덱스 버퍼 정보 입력 및 메시 생성
	static std::shared_ptr<Ext_DirectXMesh> CreateMesh(std::string_view _Name)
	{
		return CreateMesh(_Name, _Name, _Name);
	}

	// 버텍스 버퍼와 인덱스 버퍼 정보 입력 및 생성
	static std::shared_ptr<Ext_DirectXMesh> CreateMesh(std::string_view _Name, std::string_view _VBName, std::string_view _IBName)
	{
		std::shared_ptr<Ext_DirectXMesh> NewMesh = Ext_ResourceManager::CreateNameResource(_Name);
		NewMesh->VertexBufferPtr = Ext_DirectXVertexBuffer::Find(_VBName);
		NewMesh->IndexBufferPtr = Ext_DirectXIndexBuffer::Find(_IBName);

		if ((nullptr == NewMesh->VertexBufferPtr) || (nullptr == NewMesh->IndexBufferPtr))
		{
			MsgAssert("메시 생성 실패");
		}

		return NewMesh;
	}


	// Getter
	std::shared_ptr<class Ext_DirectXVertexBuffer> GetVertexBuffer() { return VertexBufferPtr; }
	std::shared_ptr<class Ext_DirectXIndexBuffer> GetIndexBuffer() { return IndexBufferPtr; }

protected:
	
private:
	D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	std::shared_ptr<class Ext_DirectXVertexBuffer> VertexBufferPtr; // 상수버퍼 데이터 저장용
	std::shared_ptr<class Ext_DirectXIndexBuffer> IndexBufferPtr; // 인덱스버퍼 데이터 저장용
	
	void MeshSetting(); // InputAssembler1(), InputAssembler2() 호출
	void InputAssembler1(); // 인풋어셈블러 1단계 실시, IASetVertexBuffers(), IASetPrimitiveTopology() 실시
	void InputAssembler2(); // 인풋어셈블러 2단계 실시, IndexBufferSetting() 호출
};