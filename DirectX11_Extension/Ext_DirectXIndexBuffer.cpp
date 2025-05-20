#include "PrecompileHeader.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXDevice.h"

void Ext_DirectXIndexBuffer::CreateIndexBuffer(const void* _Data, UINT _IndexSize, UINT _IndexCount)
{
	IndexSize = _IndexSize;

	switch (IndexSize)
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

	IndexCount = _IndexCount;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = _Data;

	BufferInfo.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferInfo.ByteWidth = IndexSize * IndexCount;

	BufferInfo.CPUAccessFlags = 0;
	if (0 == BufferInfo.CPUAccessFlags)
	{
		BufferInfo.Usage = D3D11_USAGE_DEFAULT;
	}
	else 
	{
		BufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	}

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateBuffer(&BufferInfo, &Data, Buffer.GetAddressOf()))
	{
		MsgAssert("버텍스 버퍼 생성에 실패했습니다.");
	}
}
