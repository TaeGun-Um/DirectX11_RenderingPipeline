#include "PrecompileHeader.h"
#include "Ext_DirectXPixelShader.h"
#include "Ext_DirectXDevice.h"

Ext_DirectXPixelShader::Ext_DirectXPixelShader()
{
}

Ext_DirectXPixelShader::~Ext_DirectXPixelShader()
{
}

void Ext_DirectXPixelShader::CreatePixelShader(std::string_view _Path, std::string_view _EntryPoint, UINT _VersionHigh, UINT _VersionLow)
{
	CreateVersion("ps", _VersionHigh, _VersionLow);
	EntryPoint = _EntryPoint;

	unsigned int Flag = 0;

#ifdef _DEBUG
	Flag = D3D10_SHADER_DEBUG;

#endif
	Flag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	COMPTR<ID3DBlob> Error;
	std::wstring UniCodePath = Base_String::StringToWString(_Path.data());
	if (S_OK != D3DCompileFromFile(UniCodePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, EntryPoint.c_str(), Version.c_str(), Flag, 0, BinaryCode.GetAddressOf(), Error.GetAddressOf()))
	{
		// ������ �ؽ�Ʈ�� ���
		std::string ErrorString = reinterpret_cast<char*>(Error->GetBufferPointer());
		MsgAssert(ErrorString);
		return;
	}
	// <<����>>
	/*1. ���� ��� �Է�*/
	/*2. �⺻ include ó�� ��� ����*/
	/*3. �Լ���*/
	/*4. ���̴� ��, ���־� ��Ʃ��� 2022���� �⺻ �����ϸ� Vertex Shader Model 5.0�̱� ������ vs_5_0*/
	/*5. ������ �÷��� ����*/
	/*6. ������ �÷��� ����*/
	/*7. ��� ����Ʈ�ڵ�*/
	/*8. ��� ����*/

	// �����ϵ� ����Ʈ�ڵ�� GPU�� Pixel Shader ����
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreatePixelShader(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), nullptr, PixelShader.GetAddressOf()))
	{
		MsgAssert("�ȼ� ���̴� �ڵ� ������ ����");
		return;
	}

	// ������� ����, ���ҽ� ����
	ShaderResourceSetting();
}

// PSSetShader ȣ��� �ȼ� ���̴� ����
void Ext_DirectXPixelShader::PixelShaderSetting()
{
	if (nullptr == PixelShader.Get())
	{
		MsgAssert("�ȼ� ���̴��� �������� �ʾ� ���ÿ� ����");
	}

	Ext_DirectXDevice::GetContext()->PSSetShader(PixelShader.Get(), nullptr, 0);
}