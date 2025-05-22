#include "PrecompileHeader.h"
#include "Ext_DirectXShader.h"
#include "Ext_DirectXVertexShader.h"
#include "Ext_DirectXPixelShader.h"
#include "Ext_DirectXComputeShader.h"
#include "Ext_DirectXConstantBuffer.h"

std::multimap<std::string, Setter> Ext_DirectXShader::ConstantBufferSetters; // 임시

void Ext_DirectXShader::CreateVersion(std::string_view _ShaderType, UINT _VersionHigt /*= 5*/, UINT _VersionLow /*= 0*/)
{
	// vs_5_0
	Version += _ShaderType;
	Version += "_";
	Version += std::to_string(_VersionHigt);
	Version += "_";
	Version += std::to_string(_VersionLow);
}

// EntryPoint 기준으로, 셰이터 종류를 탐색
ShaderType Ext_DirectXShader::FindShaderType(std::string_view _EntryPoint)
{
	std::string Lower = _EntryPoint.data();

	if (Lower.find("_VS") != std::string::npos) return ShaderType::Vertex;
	if (Lower.find("_PS") != std::string::npos) return ShaderType::Pixel;
	if (Lower.find("_CS") != std::string::npos) return ShaderType::Compute;
	if (Lower.find("_GS") != std::string::npos) return ShaderType::Geometry;
	// if (Lower.find("_HS") != std::string::npos) return ShaderType::Unknown; // 또는 Hull
	// if (Lower.find("_DS") != std::string::npos) return ShaderType::Unknown; // 또는 Domain

	return ShaderType::Unknown;
}

// 셰이더 종류 기준으로 컴파일 진행
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
		MsgAssert("Geometry 셰이더 타입은 아직 안만듬");
		break;
	}
	case ShaderType::Unknown:
	{
		MsgAssert("EntryPoint 탐색 실패");
		break;
	}
	}
}

void Ext_DirectXShader::ShaderResourceSetting()
{
	// 임시로 상수 버퍼 생성
	ID3D11ShaderReflection* CompileInfo = nullptr;

	// VSBlob, PSBlob : 바이너리코드
	{
		if (S_OK != D3DReflect(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&CompileInfo)))
		{
			MsgAssert("셰이더 리플렉션 실패");
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

				// std::multimap<std::string, GameEngineConstantBufferSetter> ConstantBufferSetters;에 저장 // ResHelper.CreateConstantBufferSetter(Setter);
				ConstantBufferSetters.insert(std::make_pair(Set.Name, Set));
			}
		}
	}
	{
		if (S_OK != D3DReflect(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&CompileInfo)))
		{
			MsgAssert("셰이더 리플렉션 실패");
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

				// std::multimap<std::string, GameEngineConstantBufferSetter> ConstantBufferSetters;에 저장 // ResHelper.CreateConstantBufferSetter(Setter);
				ConstantBufferSetters.insert(std::make_pair(Set.Name, Set));
			}
		}
	}
}