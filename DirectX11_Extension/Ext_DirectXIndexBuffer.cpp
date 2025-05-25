#include "PrecompileHeader.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXDevice.h"

// IndexBuffer 생성
void Ext_DirectXIndexBuffer::CreateIndexBuffer(const void* _Data, UINT _IndexSize, UINT _IndexCount)
{
	VertexSize = _IndexSize;

	switch (VertexSize)
	{
	case 2:
		Format = DXGI_FORMAT_R16_UINT;
		break;
	case 4:
		Format = DXGI_FORMAT_R32_UINT;
		break;
	default:
		break;
	}

	VertexCount = _IndexCount;

	IndexBufferInfo.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferInfo.ByteWidth = VertexSize * VertexCount;
	IndexBufferInfo.CPUAccessFlags = 0;
	if (0 == IndexBufferInfo.CPUAccessFlags)
	{
		IndexBufferInfo.Usage = D3D11_USAGE_DEFAULT;
	}
	else 
	{
		IndexBufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	}

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = _Data;

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateBuffer(&IndexBufferInfo, &Data, IndexBuffer.GetAddressOf()))
	{
		MsgAssert("버텍스 버퍼 생성에 실패했습니다.");
	}
}

void Ext_DirectXIndexBuffer::IndexBufferSetting()
{
	if (nullptr == IndexBuffer)
	{
		MsgAssert("ID3DBuffer가 만들어지지 않은 버텍스 버퍼 입니다.");
		return;
	}

	Ext_DirectXDevice::GetContext()->IASetIndexBuffer(IndexBuffer.Get(), Format, Offset);
}