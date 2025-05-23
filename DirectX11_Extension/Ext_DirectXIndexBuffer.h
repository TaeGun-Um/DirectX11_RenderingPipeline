#pragma once
#include "Ext_ResourceManager.h"

// 인덱스 버퍼(Index Buffer) 생성을 위한 클래스
class Ext_DirectXIndexBuffer : public Ext_ResourceManager<Ext_DirectXIndexBuffer>
{
public:
	// constrcuter destructer
	Ext_DirectXIndexBuffer() {}
	~Ext_DirectXIndexBuffer() {}

	// delete Function
	Ext_DirectXIndexBuffer(const Ext_DirectXIndexBuffer& _Other) = delete;
	Ext_DirectXIndexBuffer(Ext_DirectXIndexBuffer&& _Other) noexcept = delete;
	Ext_DirectXIndexBuffer& operator=(const Ext_DirectXIndexBuffer& _Other) = delete;
	Ext_DirectXIndexBuffer& operator=(Ext_DirectXIndexBuffer&& _Other) noexcept = delete;

	// IndexBuffer 생성
	template<typename Type>
	static std::shared_ptr<Ext_DirectXIndexBuffer> CreateIndexBuffer(std::string_view _Name, const std::vector<Type>& _Vertexs)
	{
		std::shared_ptr<Ext_DirectXIndexBuffer> NewIndexBuffer = Ext_ResourceManager::CreateNameResource(_Name);
		NewIndexBuffer->CreateIndexBuffer(&_Vertexs[0], sizeof(Type), static_cast<UINT>(_Vertexs.size()));

		return NewIndexBuffer;
	}

	void IndexBufferSetting();

	COMPTR<ID3D11Buffer>& GetIndexBuffer() { return IndexBuffer; }
	UINT GetVertexSize() { return VertexSize; }
	UINT GetVertexCount() { return VertexCount; }
	UINT GetBufferSize() { return IndexBufferInfo.ByteWidth; }

protected:
	
private:
	void CreateIndexBuffer(const void* _Data, UINT _IndexSize, UINT _IndexCount); // IndexBuffer 생성
	
	D3D11_BUFFER_DESC IndexBufferInfo = { 0, };  // 인덱스 버퍼 DESC 저장용
	COMPTR<ID3D11Buffer> IndexBuffer = nullptr;   // 인덱스 버퍼 인터페이스 저장용
	UINT VertexSize = 0;											// 버텍스 사이즈
	UINT VertexCount = 0;										// 버텍스 갯수
	DXGI_FORMAT Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

	UINT Offset = 0;
};