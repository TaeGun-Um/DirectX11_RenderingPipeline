#pragma once
#include "Ext_ResourceManager.h"
#include "Ext_DirectXVertex.h"

class Ext_DirectXVertexBuffer : public Ext_ResourceManager<Ext_DirectXVertexBuffer>
{
public:
	// constrcuter destructer
	Ext_DirectXVertexBuffer() {}
	~Ext_DirectXVertexBuffer() {}

	// delete Function
	//Ext_DirectXVertexBuffer(const Ext_DirectXVertexBuffer& _Other) = delete;
	//Ext_DirectXVertexBuffer(Ext_DirectXVertexBuffer&& _Other) noexcept = delete;
	//Ext_DirectXVertexBuffer& operator=(const Ext_DirectXVertexBuffer& _Other) = delete;
	//Ext_DirectXVertexBuffer& operator=(Ext_DirectXVertexBuffer&& _Other) noexcept = delete;

	// VertexBuffer 버퍼 생성
	template<typename VertexType>
	static std::shared_ptr<Ext_DirectXVertexBuffer> CreateVertexBuffer(std::string_view _Name, const std::vector<VertexType>& _Vertexs)
	{
		std::shared_ptr<Ext_DirectXVertexBuffer> NewVertexBuffer = Ext_ResourceManager::CreateNameResource(_Name);
		NewVertexBuffer->InputLayoutInfo = &VertexType::GetInputLayoutElement();
		NewVertexBuffer->CreateVertexBuffer(&_Vertexs[0], sizeof(VertexType), static_cast<UINT>(_Vertexs.size()));

		return NewVertexBuffer;
	}

	// Getter
	COMPTR<ID3D11Buffer>& GetBuffer() { return Buffer; }
	UINT GetVertexSize() { return VertexSize; }

protected:
	
private:
	void CreateVertexBuffer(const void* _Data, UINT _VertexSize, UINT _VertexCount);

	InputLayoutElement* InputLayoutInfo = nullptr;
	COMPTR<ID3D11Buffer> Buffer = nullptr;
	D3D11_BUFFER_DESC BufferInfo = { 0, };
	UINT VertexSize = 0;
	UINT VertexCount = 0;
	
};