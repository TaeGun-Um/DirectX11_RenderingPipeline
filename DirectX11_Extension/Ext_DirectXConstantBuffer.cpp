#include "PrecompileHeader.h"
#include "Ext_DirectXConstantBuffer.h"
#include "Ext_DirectXDevice.h"

std::map<int, std::map<std::string, std::shared_ptr<Ext_DirectXConstantBuffer>>> Ext_DirectXConstantBuffer::ConstantBuffers;
std::string Ext_DirectXConstantBuffer::Name = "";

Ext_DirectXConstantBuffer::Ext_DirectXConstantBuffer()
{
}

Ext_DirectXConstantBuffer::~Ext_DirectXConstantBuffer()
{
}

void Ext_DirectXConstantBuffer::CreateConstantBuffer(const D3D11_SHADER_BUFFER_DESC& _BufferDesc)
{
	ConstantBufferInfo.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferInfo.ByteWidth = _BufferDesc.Size;
	ConstantBufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	if (0 == ConstantBufferInfo.CPUAccessFlags)
	{
		ConstantBufferInfo.Usage = D3D11_USAGE_DEFAULT;
	}
	else 
	{
		ConstantBufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	}

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateBuffer(&ConstantBufferInfo, nullptr, ConstantBuffer.GetAddressOf()))
	{
		MsgAssert("상수 버퍼 생성에 실패했습니다.");
		return;
	}
}

void Ext_DirectXConstantBuffer::ChangeData(const void* _Data, UINT _Size)
{
	// 머티리얼들은 상수버퍼나 이런걸 통일해 놓은 것이다.
	if (nullptr == _Data)
	{
		std::string CurName = Name;
		MsgAssert(CurName + "에 nullptr인 데이터를 세팅하려고 했습니다.");
		return;
	}

	if (ConstantBufferInfo.ByteWidth != _Size)
	{
		std::string CurName = Name;
		MsgAssert(CurName + "크기가 다른 데이터가 들어왔습니다.");
		return;
	}

	D3D11_MAPPED_SUBRESOURCE SettingResources = { 0, };

	// 그래픽카드야 너한테 보낼께있어 잠깐 멈춰봐 
	// D3D11_MAP_WRITE_DISCARD 최대한 빠르게 처리하는 
	Ext_DirectXDevice::GetContext()->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SettingResources);

	if (SettingResources.pData == nullptr)
	{
		std::string CurName = Name;
		MsgAssert(CurName + " 그래픽카드에게 메모리 접근을 허가받지 못했습니다.");
		return;
	}
	memcpy_s(SettingResources.pData, ConstantBufferInfo.ByteWidth, _Data, ConstantBufferInfo.ByteWidth);
	Ext_DirectXDevice::GetContext()->Unmap(ConstantBuffer, 0);

	int a = 0;
}

void Ext_DirectXConstantBuffer::VSSetting(UINT _Slot)
{
	Ext_DirectXDevice::GetContext()->VSSetConstantBuffers(_Slot, 1, &ConstantBuffer);
}

void Ext_DirectXConstantBuffer::PSSetting(UINT _Slot)
{
	Ext_DirectXDevice::GetContext()->PSSetConstantBuffers(_Slot, 1, &ConstantBuffer);
}

void Ext_DirectXConstantBuffer::CSSetting(UINT _Slot)
{
	Ext_DirectXDevice::GetContext()->CSSetConstantBuffers(_Slot, 1, &ConstantBuffer);
}