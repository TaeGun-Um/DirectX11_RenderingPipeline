#include "PrecompileHeader.h"
#include "Ext_DirectXShader.h"
#include "Ext_DirectXVertexShader.h"
#include "Ext_DirectXPixelShader.h"
#include "Ext_DirectXComputeShader.h"
#include "Ext_DirectXConstantBuffer.h"

std::multimap<std::string, Setter> Ext_DirectXShader::ConstantBufferSetters; // �ӽ�

void Ext_DirectXShader::CreateVersion(std::string_view _ShaderType, UINT _VersionHigt /*= 5*/, UINT _VersionLow /*= 0*/)
{
	// vs_5_0
	Version += _ShaderType;
	Version += "_";
	Version += std::to_string(_VersionHigt);
	Version += "_";
	Version += std::to_string(_VersionLow);
}

// EntryPoint ��������, ������ ������ Ž��
ShaderType Ext_DirectXShader::FindShaderType(std::string_view _EntryPoint)
{
	std::string Lower = _EntryPoint.data();

	if (Lower.find("_VS") != std::string::npos) return ShaderType::Vertex;
	if (Lower.find("_PS") != std::string::npos) return ShaderType::Pixel;
	if (Lower.find("_CS") != std::string::npos) return ShaderType::Compute;
	if (Lower.find("_GS") != std::string::npos) return ShaderType::Geometry;
	// if (Lower.find("_HS") != std::string::npos) return ShaderType::Unknown; // �Ǵ� Hull
	// if (Lower.find("_DS") != std::string::npos) return ShaderType::Unknown; // �Ǵ� Domain

	return ShaderType::Unknown;
}

// ���̴� ���� �������� ������ ����
void Ext_DirectXShader::ShaderAutoCompile(std::string_view _Path, std::string_view _EntryPoint)
{
	ShaderType Type = FindShaderType(_EntryPoint);

	switch (Type)
	{
	case ShaderType::Vertex:
	{
		Ext_DirectXVertexShader::LoadVertexShader(_Path, _EntryPoint);
		break;
	}
	case ShaderType::Pixel:
	{
		Ext_DirectXPixelShader::LoadPixelShader(_Path, _EntryPoint);
		break;
	}
	case ShaderType::Compute:
	{
		Ext_DirectXComputeShader::LoadComputeShader(_Path, _EntryPoint);
		break;
	}
	case ShaderType::Geometry:
	{
		MsgAssert("Geometry ���̴� Ÿ���� ���� �ȸ���");
		break;
	}
	case ShaderType::Unknown:
	{
		MsgAssert("EntryPoint Ž�� ����");
		break;
	}
	}
}

void Ext_DirectXShader::ShaderResourceSetting()
{
	// �ӽ÷� ��� ���� ����
	ID3D11ShaderReflection* CompileInfo = nullptr;

	// VSBlob, PSBlob : ���̳ʸ��ڵ�
	{
		if (S_OK != D3DReflect(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&CompileInfo)))
		{
			MsgAssert("���̴� ���÷��� ����");
			return;
		}

		D3D11_SHADER_DESC ShaderDesc;
		CompileInfo->GetDesc(&ShaderDesc);
		D3D11_SHADER_INPUT_BIND_DESC ResDesc;

		for (UINT i = 0; i < ShaderDesc.BoundResources; i++)
		{
			CompileInfo->GetResourceBindingDesc(i, &ResDesc);
			std::string Name = ResDesc.Name;
			D3D_SHADER_INPUT_TYPE Type = ResDesc.Type;
			std::string UpperName = Base_String::ToUpper(ResDesc.Name);

			if (Type == D3D_SIT_CBUFFER)
			{
				ID3D11ShaderReflectionConstantBuffer* CBufferPtr = CompileInfo->GetConstantBufferByName(ResDesc.Name);

				D3D11_SHADER_BUFFER_DESC BufferDesc;
				CBufferPtr->GetDesc(&BufferDesc);

				std::shared_ptr<Ext_DirectXConstantBuffer> Res = Ext_DirectXConstantBuffer::CreateConstantBuffer(UpperName, BufferDesc, BufferDesc.Size);

				Setter Set;
				// Set.ParentShader = this;
				Set.Name = UpperName;
				Set.BindPoint = ResDesc.BindPoint;
				Set.Res = Res;

				// std::multimap<std::string, GameEngineConstantBufferSetter> ConstantBufferSetters;�� ���� // ResHelper.CreateConstantBufferSetter(Setter);
				ConstantBufferSetters.insert(std::make_pair(Set.Name, Set));
			}
		}
	}
	{
		if (S_OK != D3DReflect(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&CompileInfo)))
		{
			MsgAssert("���̴� ���÷��� ����");
			return;
		}

		D3D11_SHADER_DESC ShaderDesc;
		CompileInfo->GetDesc(&ShaderDesc);
		D3D11_SHADER_INPUT_BIND_DESC ResDesc;

		for (UINT i = 0; i < ShaderDesc.BoundResources; i++)
		{
			CompileInfo->GetResourceBindingDesc(i, &ResDesc);
			std::string Name = ResDesc.Name;
			D3D_SHADER_INPUT_TYPE Type = ResDesc.Type;
			std::string UpperName = Base_String::ToUpper(ResDesc.Name);

			if (Type == D3D_SIT_CBUFFER)
			{
				ID3D11ShaderReflectionConstantBuffer* CBufferPtr = CompileInfo->GetConstantBufferByName(ResDesc.Name);

				D3D11_SHADER_BUFFER_DESC BufferDesc;
				CBufferPtr->GetDesc(&BufferDesc);

				std::shared_ptr<Ext_DirectXConstantBuffer> Res = Ext_DirectXConstantBuffer::CreateConstantBuffer(UpperName, BufferDesc, BufferDesc.Size);

				Setter Set;
				// Set.ParentShader = this;
				Set.Name = UpperName;
				Set.BindPoint = ResDesc.BindPoint;
				Set.Res = Res;

				// std::multimap<std::string, GameEngineConstantBufferSetter> ConstantBufferSetters;�� ���� // ResHelper.CreateConstantBufferSetter(Setter);
				ConstantBufferSetters.insert(std::make_pair(Set.Name, Set));
			}
		}
	}
}