#pragma once
#include "Ext_ResourceManager.h"

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

	// IndexBuffer »ý¼º
	template<typename T>
	static std::shared_ptr<Ext_DirectXIndexBuffer> CreateIndexBuffer(std::string_view _Name, const std::vector<T>& _Vertexs)
	{
		std::shared_ptr<Ext_DirectXIndexBuffer> NewIndexBuffer = Ext_ResourceManager::CreateNameResource(_Name);
		NewIndexBuffer->CreateIndexBuffer(&_Vertexs[0], sizeof(T), static_cast<UINT>(_Vertexs.size()));

		return NewIndexBuffer;
	}

protected:
	
private:
	void CreateIndexBuffer(const void* _Data, UINT _IndexSize, UINT _IndexCount);
	
	DXGI_FORMAT Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
	D3D11_BUFFER_DESC BufferInfo = { 0, };
	COMPTR<ID3D11Buffer> Buffer = nullptr;
	UINT IndexSize = 0;
	UINT IndexCount = 0;
};