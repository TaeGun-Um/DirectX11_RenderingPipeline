#include "PrecompileHeader.h"
#include "Ext_DirectXMesh.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXDevice.h"

Ext_DirectXMesh::Ext_DirectXMesh()
{
}

Ext_DirectXMesh::~Ext_DirectXMesh()
{
}

void Ext_DirectXMesh::MeshSetting()
{
	InputAssembler1();
	InputAssembler2();
}

void Ext_DirectXMesh::InputAssembler1()
{
	if (nullptr == VertexBufferPtr)
	{
		MsgAssert("���ؽ� ���۰� �������� �ʾƼ� ��ǲ�����1 ������ ������ �� �����ϴ�.");
		return;
	}

	VertexBufferPtr->VertexBufferSetting();

	Ext_DirectXDevice::GetContext()->IASetPrimitiveTopology(Topology);
}

void Ext_DirectXMesh::InputAssembler2()
{
	// �׸��� ������ ���� �����͸� �־��ش� // 012023
	if (nullptr == IndexBufferPtr)
	{
		MsgAssert("�ε��� ���۰� �������� �ʾƼ� ��ǲ �����2 ������ ������ �� �����ϴ�.");
		return;
	}

	IndexBufferPtr->IndexBufferSetting();
}