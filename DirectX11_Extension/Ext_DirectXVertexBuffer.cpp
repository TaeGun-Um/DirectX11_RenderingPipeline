#include "PrecompileHeader.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXDevice.h"

void Ext_DirectXVertexBuffer::CreateVertexBuffer(const void* _Data, UINT _VertexSize, UINT _VertexCount)
{
	VertexSize = _VertexSize;
	VertexCount = _VertexCount;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = _Data;

	BufferInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferInfo.ByteWidth = VertexSize * VertexCount;

	BufferInfo.CPUAccessFlags = 0;

	if (0 == BufferInfo.CPUAccessFlags)
	{
		BufferInfo.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		BufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	}

	// VertexBuffer 생성
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateBuffer(&BufferInfo, &Data, Buffer.GetAddressOf()))
	{
		MsgAssert("버텍스 버퍼 생성에 실패했습니다.");
	}
}
