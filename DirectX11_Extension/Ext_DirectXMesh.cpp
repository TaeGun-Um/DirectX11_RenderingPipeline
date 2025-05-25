#include "PrecompileHeader.h"
#include "Ext_DirectXMesh.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXDevice.h"

void Ext_DirectXMesh::MeshSetting()
{
	InputAssembler1();
	InputAssembler2();
}

// 정점 버퍼 / 도형 종류 설정
void Ext_DirectXMesh::InputAssembler1()
{
	if (nullptr == VertexBufferPtr)
	{
		MsgAssert("버텍스 버퍼가 존재하지 않아서 인풋어셈블러1 과정을 실행할 수 없습니다.");
		return;
	}

	VertexBufferPtr->VertexBufferSetting(); // 정점 버퍼 설정
 
	Ext_DirectXDevice::GetContext()->IASetPrimitiveTopology(Topology); // 도형 종류 설정
	// GPU에 삼각형 리스트, 라인 스트립, 포인트 리스트 등을 설정
	// 정점들을 삼각형으로 해석하도록 하기 위해 D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST 전달
}

// 인덱스 버퍼 설정
void Ext_DirectXMesh::InputAssembler2()
{
	// 그리는 순서에 대한 데이터를 넣어준다 // 012023
	if (nullptr == IndexBufferPtr)
	{
		MsgAssert("인덱스 버퍼가 존재하지 않아서 인풋 어셈블러2 과정을 실행할 수 없습니다.");
		return;
	}

	IndexBufferPtr->IndexBufferSetting(); // 인덱스 버퍼 설정
}