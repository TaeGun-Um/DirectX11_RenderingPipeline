#pragma once
#include "Ext_ResourceManager.h"

// ��� ����(Constant Buffer) ������ ���� Ŭ����
class Ext_DirectXConstantBuffer : public Ext_ResourceManager<Ext_DirectXConstantBuffer>
{
public:
	// constrcuter destructer
	Ext_DirectXConstantBuffer();
	~Ext_DirectXConstantBuffer();

	// delete Function
	Ext_DirectXConstantBuffer(const Ext_DirectXConstantBuffer& _Other) = delete;
	Ext_DirectXConstantBuffer(Ext_DirectXConstantBuffer&& _Other) noexcept = delete;
	Ext_DirectXConstantBuffer& operator=(const Ext_DirectXConstantBuffer& _Other) = delete;
	Ext_DirectXConstantBuffer& operator=(Ext_DirectXConstantBuffer&& _Other) noexcept = delete;

	// ConstantBuffer ����
	static std::shared_ptr<Ext_DirectXConstantBuffer> CreateConstantBuffer(std::string_view _Name, const D3D11_SHADER_BUFFER_DESC& _Desc, int _Byte)
	{
		std::string NewName = Base_String::ToUpper(_Name.data());
		if (ConstantBuffers[_Byte].end() != ConstantBuffers[_Byte].find(NewName))
		{
			return ConstantBuffers[_Byte][NewName];
		}

		std::shared_ptr<Ext_DirectXConstantBuffer> NewIndexBuffer = Ext_ResourceManager::CreateResource();
		NewIndexBuffer->CreateConstantBuffer(_Desc);
		Name = NewName;

		return NewIndexBuffer;
	}

	COMPTR<ID3D11Buffer>& GetConstantBuffer() { return ConstantBuffer; }

	void ChangeData(const void* _Data, UINT _Size);
	void VSSetting(UINT _Slot);
	void PSSetting(UINT _Slot);
	void CSSetting(UINT _Slot);

protected:
	
private:
	void CreateConstantBuffer(const D3D11_SHADER_BUFFER_DESC& _BufferDesc);

	static std::map<int, std::map<std::string, std::shared_ptr<Ext_DirectXConstantBuffer>>> ConstantBuffers;
	static std::string Name;
	D3D11_BUFFER_DESC ConstantBufferInfo = { 0, };  // ��� ���� DESC �����
	COMPTR<ID3D11Buffer> ConstantBuffer = nullptr;   // ��� ���� �������̽� �����

};