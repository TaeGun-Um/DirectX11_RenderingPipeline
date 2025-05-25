#include "PrecompileHeader.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXDevice.h"

// 버텍스 버퍼 생성
void Ext_DirectXVertexBuffer::CreateVertexBuffer(const void* _Data, UINT _VertexSize, UINT _VertexCount)
{
	VertexSize = _VertexSize;
	VertexCount = _VertexCount;

	// D3D11_BUFFER_DESC 정보 입력
	VertexBufferInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferInfo.ByteWidth = VertexSize * VertexCount;
	VertexBufferInfo.CPUAccessFlags = 0;
	if (0 == VertexBufferInfo.CPUAccessFlags)
	{
		VertexBufferInfo.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		VertexBufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	}
	// <<설명>>
	/*1. BindFlags : 이 버퍼가 GPU에서 어떤 용도로 바인딩될 지 설명하기 위함, D3D11_BIND_VERTEX_BUFFER는 버텍스 버퍼로 사용하겠다는 뜻*/ 
	/*2. ByteWidth : 버퍼의 크기(byte 단위), (Vertex 크기 * Vertex 갯수)로 정해줌*/
	/*3. CPUAccessFlags : CPU가 버퍼에 접근할 수 있는지 설정*/
	/*4. Usage : 버퍼의 사용 방식을 정의함, D3D11_USAGE_DEFAULT는 GPU가 사용하는 버퍼로 설정하여 CPU접근 불가 설정, D3D11_USAGE_DYNAMIC은 CPU가 자주 수정하는 버퍼*/

	// D3D11_SUBRESOURCE_DATA : 버퍼 또는 리소스를 생성할 때 초기 데이터를 GPU에 전달하기 위한 구조체
	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = _Data;
	// pSysMem은 리소스에 복사할 원본 데이터의 포인터
	// 두 개 더 있는데, 버퍼 생성시에는 하지않음

	// VertexBuffer 생성
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateBuffer(&VertexBufferInfo, &Data, VertexBuffer.GetAddressOf()))
	{
		MsgAssert("버텍스 버퍼 생성에 실패했습니다.");
	}
	// <<설명>>
	/*1. D3D11_BUFFER_DESC 전달*/
	/*2. D3D11_SUBRESOURCE_DATA 전달*/
	/*3. ID3D11Buffer 전달*/
}

// 정점 버퍼 종류 설정
void Ext_DirectXVertexBuffer::VertexBufferSetting()
{
	if (nullptr == VertexBuffer)
	{
		MsgAssert("버텍스 버퍼가 없어 세팅 불가");
		return;
	}

	Ext_DirectXDevice::GetContext()->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &VertexSize, &Offset);
	// 1. 가본 정점 슬롯
	// 2. 버퍼 하나만 사용
	// 3. 버텍스 버퍼 주소
	// 4. 정점 하나당 크기(Stride)
	// 5. 버퍼 시작 오프셋(일반적으로 0)
}
