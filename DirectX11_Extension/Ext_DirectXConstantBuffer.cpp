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
	}
}
