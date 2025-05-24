#include "PrecompileHeader.h"
#include "Ext_DirectXComputeShader.h"
#include "Ext_DirectXDevice.h"

Ext_DirectXComputeShader::Ext_DirectXComputeShader()
{
	Type = ShaderType::Compute;
}

Ext_DirectXComputeShader::~Ext_DirectXComputeShader()
{
}

void Ext_DirectXComputeShader::CreateComputeShader(std::string_view _Path, std::string_view _EntryPoint, UINT _VersionHigh, UINT _VersionLow)
{
	CreateVersion("cs", _VersionHigh, _VersionLow);
	EntryPoint = _EntryPoint;

	unsigned int Flag = 0;

#ifdef _DEBUG
	Flag = D3D10_SHADER_DEBUG;

#endif

	Flag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
	// Flag & D3DCOMPILE_SKIP_VALIDATION
	// �ɼ��� �������� Ȯ���ϴ¹�
	// Flag |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

	ID3DBlob* Error;

	std::wstring UniCodePath = Base_String::AnsiToUniCode(_Path);
	if (S_OK != D3DCompileFromFile(UniCodePath.c_str(),	nullptr,	D3D_COMPILE_STANDARD_FILE_INCLUDE,	EntryPoint.c_str(),	Version.c_str(),	Flag,	0,	BinaryCode.GetAddressOf(), &Error))
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

	// �����ϵ� ����Ʈ�ڵ�� GPU�� Compute Shader ����
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateComputeShader(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), nullptr, ComputeShader.GetAddressOf()))
	{
		MsgAssert("��ǻƮ ���̴� �ڵ� ������ ����");
		return;
	}

	// ������� ����, ���ҽ� ����
	ShaderResourceSetting();
}

// CSSetShader ȣ��� ��ǻƮ ���̴� ����
void Ext_DirectXComputeShader::ComputeShaderSetting()
{
	if (nullptr == ComputeShader.Get())
	{
		MsgAssert("��ǻƮ ���̴��� �������� �ʾ� ���ÿ� ����");
	}

	Ext_DirectXDevice::GetContext()->CSSetShader(ComputeShader.Get(), nullptr, 0);
}