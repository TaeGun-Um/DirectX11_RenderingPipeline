#include "PrecompileHeader.h"
#include "Ext_DirectXShader.h"
#include "Ext_DirectXVertexShader.h"
#include "Ext_DirectXPixelShader.h"
#include "Ext_DirectXComputeShader.h"
#include "Ext_DirectXConstantBuffer.h"
#include "Ext_DirectXBufferSetter.h"

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

// 상수 버퍼 세팅
void Ext_DirectXShader::ShaderResourceSetting()
{
	if (nullptr == BinaryCode)
	{
		MsgAssert("쉐이더가 컴파일 되지 않아서 쉐이더의 리소스를 조사할 수 없습니다.");
		return;
	}

	// Reflection 
	// RTTI의 비슷한 개념으로 
	ID3D11ShaderReflection* CompileInfo = nullptr;

	if (S_OK != D3DReflect(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&CompileInfo)))
	{
		MsgAssert("쉐이더 리플렉션에 실패했습니다.");
		return;
	}

	D3D11_SHADER_DESC Info;
	CompileInfo->GetDesc(&Info);
	D3D11_SHADER_INPUT_BIND_DESC ResDesc;

	// 내가 사용한 상수버퍼 텍스처 샘플러등의 총합입니다.
	for (UINT i = 0; i < Info.BoundResources; i++)
	{
		// 리소스 정보를 얻어오게 되고
		CompileInfo->GetResourceBindingDesc(i, &ResDesc);
		std::string Name = ResDesc.Name;
		D3D_SHADER_INPUT_TYPE Type = ResDesc.Type;
		std::string UpperName = Base_String::ToUpper(ResDesc.Name);

		switch (Type)
		{
		case D3D_SIT_CBUFFER:
		{
			ID3D11ShaderReflectionConstantBuffer* CBufferPtr = CompileInfo->GetConstantBufferByName(ResDesc.Name);

			D3D11_SHADER_BUFFER_DESC BufferDesc;
			CBufferPtr->GetDesc(&BufferDesc);

			std::shared_ptr<Ext_DirectXConstantBuffer> ConstantBuffer = Ext_DirectXConstantBuffer::CreateConstantBuffer(UpperName, BufferDesc, BufferDesc.Size);

			ConstantBufferSetter Set;
			Set.OwnerShader = GetSharedFromThis<Ext_DirectXShader>();
			Set.Name = UpperName;
			Set.BindPoint = ResDesc.BindPoint;
			Set.ConstantBuffer = ConstantBuffer;

			// std::multimap<std::string, GameEngineConstantBufferSetter> ConstantBufferSetters;에 저장
			BufferSetter.InsertConstantBufferSetter(Set);

			break;
		}
		case D3D_SIT_TEXTURE:
		{
			if (9 == ResDesc.BindPoint)
			{
				continue;
			}

			if (D3D_SRV_DIMENSION_TEXTURECUBE == ResDesc.Dimension)
			{
				//std::shared_ptr<GameEngineTexture> Res = GameEngineTexture::Find("BaseCubemap");

				//GameEngineTextureSetter Setter;
				//Setter.ParentShader = this;
				//Setter.Name = UpperName;
				//Setter.BindPoint = ResDesc.BindPoint;
				//Setter.Res = Res;

				//ResHelper.CreateTextureSetter(Setter);
			}
			else
			{
				//std::shared_ptr<GameEngineTexture> Res = GameEngineTexture::Find("EngineBaseTex.png");

				//GameEngineTextureSetter Setter;
				//Setter.ParentShader = this;
				//Setter.Name = UpperName;
				//Setter.BindPoint = ResDesc.BindPoint;
				//Setter.Res = Res;

				//ResHelper.CreateTextureSetter(Setter);
			}

			break;
		}
		case D3D_SIT_SAMPLER:
		{
			//std::shared_ptr<GameEngineSampler> Res = GameEngineSampler::Find(UpperName);

			//if (nullptr == Res)
			//{
			//	Res = GameEngineSampler::Find("ENGINEBASE");
			//	// MsgAssert("다음의 샘플러가 존재하지 않아서 쉐이더에 세팅해줄수가 없습니다. : " + UpperName);
			//}

			//GameEngineSamplerSetter Setter;
			//Setter.ParentShader = this;
			//Setter.Name = UpperName;
			//Setter.BindPoint = ResDesc.BindPoint;
			//Setter.Res = Res;

			//ResHelper.CreateSamplerSetter(Setter);
			break;
		}
		case D3D_SIT_STRUCTURED:
		{
			// 스트럭처드 버퍼는 텍스처 슬롯을 사용합니다.
			// 기본적으로 텍스처로 판단
			//ID3D11ShaderReflectionConstantBuffer* SBufferPtr = CompileInfo->GetConstantBufferByName(ResDesc.Name);
			//D3D11_SHADER_BUFFER_DESC BufferDesc;
			//SBufferPtr->GetDesc(&BufferDesc);

			//std::shared_ptr<GameEngineStructuredBuffer> Res = GameEngineStructuredBuffer::CreateAndFind(Name, BufferDesc, 0);

			//GameEngineStructuredBufferSetter Setter;
			//Setter.ParentShader = this;
			//Setter.Name = UpperName;
			//Setter.BindPoint = ResDesc.BindPoint;
			//Setter.Res = Res;

			//ResHelper.CreateStructuredBufferSetter(Setter);
			break;
		}
		case D3D_SIT_UAV_RWSTRUCTURED:
		{
			//ID3D11ShaderReflectionConstantBuffer* SBufferPtr = CompileInfo->GetConstantBufferByName(ResDesc.Name);
			//D3D11_SHADER_BUFFER_DESC BufferDesc;
			//SBufferPtr->GetDesc(&BufferDesc);

			//// RW버퍼로 

			//std::shared_ptr<GameEngineStructuredBuffer> Res = GameEngineStructuredBuffer::CreateAndFind(Name, BufferDesc, 0);

			//GameEngineStructuredBufferSetter Setter;
			//Setter.ParentShader = this;
			//Setter.Name = UpperName;
			//Setter.BindPoint = ResDesc.BindPoint;
			//Setter.Res = Res;

			//ResHelper.CreateStructuredBufferSetter(Setter);
			break;
		}
		default:
			MsgAssert(std::string(ResDesc.Name) + "처리할수 없는 리소스 입니다");
			break;
		}

	}
}