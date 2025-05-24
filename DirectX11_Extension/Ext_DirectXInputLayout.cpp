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
	Relase(); // �ѹ� ����� CreateInputLayout�� ���� ������

	if (nullptr == _VertexBuffer->GetInputLayout())
	{
		MsgAssert("���̾ƿ� ������ ����� ���� ���ؽ� ���� �Դϴ�.");
	}

	const std::vector<D3D11_INPUT_ELEMENT_DESC>& LayOutInfo = _VertexBuffer->GetInputLayout()->GetInputLayoutDescs();

	// CreateInputLayout�� ���� ���� ������ ���̴� �Է� ���� ���� ������ ����
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
		MsgAssert("�������� ���� ��ǲ���̾ƿ��� �����Ϸ��� �߽��ϴ�.");
	}

	Ext_DirectXDevice::GetContext()->IASetInputLayout(InputLayout);
}