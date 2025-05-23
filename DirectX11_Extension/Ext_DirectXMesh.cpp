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
		MsgAssert("버텍스 버퍼가 존재하지 않아서 인풋어셈블러1 과정을 실행할 수 없습니다.");
		return;
	}

	VertexBufferPtr->VertexBufferSetting();

	Ext_DirectXDevice::GetContext()->IASetPrimitiveTopology(Topology);
}

void Ext_DirectXMesh::InputAssembler2()
{
	// 그리는 순서에 대한 데이터를 넣어준다 // 012023
	if (nullptr == IndexBufferPtr)
	{
		MsgAssert("인덱스 버퍼가 존재하지 않아서 인풋 어셈블러2 과정을 실행할 수 없습니다.");
		return;
	}

	IndexBufferPtr->IndexBufferSetting();
}