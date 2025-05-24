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

	// CreateInputLayout은 정점 버퍼 구조와 셰이더 입력 구조 간의 매핑을 정의
	HRESULT hr = Ext_DirectXDevice::GetDevice()->CreateInputLayout
	(
		&LayOutInfo[0],
		static_cast<UINT>(LayOutInfo.size()),
		_VertexShader->GetBinaryCode()->GetBufferPointer(),
		_VertexShader->GetBinaryCode()->GetBufferSize(),
		&InputLayout
	);

	if (S_OK != hr)
	{
		char Buffer[256] = {};
		sprintf_s(Buffer, "CreateInputLayout failed: 0x%08X", hr);
		MsgAssert(Buffer)
		return;
	}
}

void Ext_DirectXInputLayout::InputLayoutSetting()
{
	if (nullptr == InputLayout)
	{
		MsgAssert("생성되지 않은 인풋레이아웃을 세팅하려고 했습니다.");
	}

	Ext_DirectXDevice::GetContext()->IASetInputLayout(InputLayout);
}