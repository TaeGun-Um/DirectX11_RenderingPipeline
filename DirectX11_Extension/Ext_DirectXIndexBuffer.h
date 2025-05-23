#pragma once
#include "Ext_ResourceManager.h"

// �ε��� ����(Index Buffer) ������ ���� Ŭ����
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

	// IndexBuffer ����
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
	void CreateIndexBuffer(const void* _Data, UINT _IndexSize, UINT _IndexCount); // IndexBuffer ����
	
	D3D11_BUFFER_DESC IndexBufferInfo = { 0, };  // �ε��� ���� DESC �����
	COMPTR<ID3D11Buffer> IndexBuffer = nullptr;   // �ε��� ���� �������̽� �����
	UINT VertexSize = 0;											// ���ؽ� ������
	UINT VertexCount = 0;										// ���ؽ� ����
	DXGI_FORMAT Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

	UINT Offset = 0;
};