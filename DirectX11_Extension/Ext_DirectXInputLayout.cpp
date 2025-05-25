#include "PrecompileHeader.h"
#include "Ext_DirectXInputLayout.h"
#include "Ext_DirectXDevice.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXVertexShader.h"
#include "Ext_DirectXVertexData.h"

void Ext_DirectXInputLayout::Relase()
{
	InputLayout.Reset();
}

void Ext_DirectXInputLayout::CreateInputLayout(std::shared_ptr<Ext_DirectXVertexBuffer> _VertexBuffer, std::shared_ptr< Ext_DirectXVertexShader> _VertexShader)
{
	Relase(); // 한번 해줘야 CreateInputLayout가 정상 동작함

	if (nullptr == _VertexBuffer->GetInputLayout())
	{
		MsgAssert("레이아웃 정보를 만들수 없는 버텍스 버퍼 입니다.");
	}

	const std::vector<D3D11_INPUT_ELEMENT_DESC>& LayOutInfo = _VertexBuffer->GetInputLayout()->GetInputLayoutDescs();

	// 정점 버퍼의 각 요소(Vertex Data)를 정점 셰이더의 입력 파라미터와 매핑할 수 있도록 연결하는 구조를 만드는 함수
	HRESULT hr = Ext_DirectXDevice::GetDevice()->CreateInputLayout
	(
		&LayOutInfo[0],
		static_cast<UINT>(LayOutInfo.size()),
		_VertexShader->GetBinaryCode()->GetBufferPointer(),
		_VertexShader->GetBinaryCode()->GetBufferSize(),
		&InputLayout
	);
	// <<설명>>
	/*1. const D3D11_INPUT_ELEMENT_DESC* : 정점 입력 요소 배열의 시작 주소, 각 요소가 어떤 데이터인지 정의*/
	/*2. UINT : 위 배열의 요소 수(입력 요소가 몇개인가)*/
	/*3. const void* : 정점 셰이더의 바이트 코드 시작 주소, 여기에는 입력 시그니처가 포함되어 있어야 함*/
	/*4. SIZE_T : 위 바이트코드의 길이*/
	/*5. ID3D11InputLayout** : 호출의 결과로 생성될 InputLayout 객체의 포인터 주소 저장->IASetInputLayout에서 활용*/

	if (S_OK != hr)
	{
		char Buffer[256] = {};
		sprintf_s(Buffer, "CreateInputLayout failed: 0x%08X", hr);
		MsgAssert(Buffer)
		return;
	}
}

// 입력 조립기 단계 중 정점 입력 레이아웃을 설정하는 함수, GPU가 정점 버퍼의 데이터를 올바르게 해석하기 위해 필수
void Ext_DirectXInputLayout::InputLayoutSetting()
{
	if (nullptr == InputLayout)
	{
		MsgAssert("생성되지 않은 인풋레이아웃을 세팅하려고 했습니다.");
	}

	Ext_DirectXDevice::GetContext()->IASetInputLayout(InputLayout);
	// POSITION, COLOR, TEXTCOOR, NORMAL 등 정보를 GPU에 알려줘서 셰이더와 정점 데이터가 정확하게 매칭되도록 함
}