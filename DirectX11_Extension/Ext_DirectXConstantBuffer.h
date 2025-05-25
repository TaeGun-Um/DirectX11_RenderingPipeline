#pragma once
#include "Ext_ResourceManager.h"

// 상수 버퍼(Constant Buffer) 생성을 위한 클래스
class Ext_DirectXConstantBuffer : public Ext_ResourceManager<Ext_DirectXConstantBuffer>
{
	friend class Ext_DirectXBufferSetter;
	friend struct ConstantBufferSetter;

public:
	// constrcuter destructer
	Ext_DirectXConstantBuffer() {}
	~Ext_DirectXConstantBuffer() {}

	// delete Function
	Ext_DirectXConstantBuffer(const Ext_DirectXConstantBuffer& _Other) = delete;
	Ext_DirectXConstantBuffer(Ext_DirectXConstantBuffer&& _Other) noexcept = delete;
	Ext_DirectXConstantBuffer& operator=(const Ext_DirectXConstantBuffer& _Other) = delete;
	Ext_DirectXConstantBuffer& operator=(Ext_DirectXConstantBuffer&& _Other) noexcept = delete;

	// ConstantBuffer 생성
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

	// Getter, Setter
	COMPTR<ID3D11Buffer>& GetConstantBuffer() { return ConstantBuffer; }
	UINT GetBufferSize() { return ConstantBufferInfo.ByteWidth; }

protected:
	
private:
	void CreateConstantBuffer(const D3D11_SHADER_BUFFER_DESC& _BufferDesc); // CreateBuffer() 호출
	void ChangeData(const void* _Data, UINT _Size); // Map() -> memcpy_s() -> Unmap() 실시
	void VSSetting(UINT _Slot); // VSSetConstantBuffers() 호출
	void PSSetting(UINT _Slot); // PSSetConstantBuffers() 호출
	void CSSetting(UINT _Slot); // CSSetConstantBuffers() 호출

	static std::map<int, std::map<std::string, std::shared_ptr<Ext_DirectXConstantBuffer>>> ConstantBuffers;
	static std::string Name;
	D3D11_BUFFER_DESC ConstantBufferInfo = { 0, };  // 상수 버퍼 DESC 저장용
	COMPTR<ID3D11Buffer> ConstantBuffer = nullptr;   // 상수 버퍼 인터페이스 저장용

};