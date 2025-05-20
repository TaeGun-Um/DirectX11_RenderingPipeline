#include "PrecompileHeader.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXDevice.h"

// ���ؽ� ���� ����
void Ext_DirectXVertexBuffer::CreateVertexBuffer(const void* _Data, UINT _VertexSize, UINT _VertexCount)
{
	VertexSize = _VertexSize;
	VertexCount = _VertexCount;

	// D3D11_BUFFER_DESC ���� �Է�
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
	// <<����>>
	/*1. BindFlags : �� ���۰� GPU���� � �뵵�� ���ε��� �� �����ϱ� ����, D3D11_BIND_VERTEX_BUFFER�� ���ؽ� ���۷� ����ϰڴٴ� ��*/ 
	/*2. ByteWidth : ������ ũ��(byte ����), (Vertex ũ�� * Vertex ����)�� ������*/
	/*3. CPUAccessFlags : CPU�� ���ۿ� ������ �� �ִ��� ����*/
	/*4. Usage : ������ ��� ����� ������, D3D11_USAGE_DEFAULT�� GPU�� ����ϴ� ���۷� �����Ͽ� CPU���� �Ұ� ����, D3D11_USAGE_DYNAMIC�� CPU�� ���� �����ϴ� ����*/

	// D3D11_SUBRESOURCE_DATA : ���� �Ǵ� ���ҽ��� ������ �� �ʱ� �����͸� GPU�� �����ϱ� ���� ����ü
	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = _Data;
	// pSysMem�� ���ҽ��� ������ ���� �������� ������
	// �� �� �� �ִµ�, ���� �����ÿ��� ��������

	// VertexBuffer ����
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateBuffer(&VertexBufferInfo, &Data, VertexBuffer.GetAddressOf()))
	{
		MsgAssert("���ؽ� ���� ������ �����߽��ϴ�.");
	}
	// <<����>>
	/*1. D3D11_BUFFER_DESC ����*/
	/*2. D3D11_SUBRESOURCE_DATA ����*/
	/*3. ID3D11Buffer ����*/
}
