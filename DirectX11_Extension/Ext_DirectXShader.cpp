#include "PrecompileHeader.h"
#include "Ext_DirectXShader.h"
#include "Ext_DirectXVertexShader.h"
#include "Ext_DirectXPixelShader.h"
#include "Ext_DirectXComputeShader.h"
#include "Ext_DirectXBufferSetter.h"
#include "Ext_DirectXConstantBuffer.h"
#include "Ext_DirectXSampler.h"
#include "Ext_DirectXTexture.h"

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
	ID3D11ShaderReflection* CompileInfo = nullptr;

	// 셰이더 바이트코드를 리플렉션하여 셰이더 내부의 입력 구조/상수버퍼/리소스 슬롯 등을 추출할 수 있는 객체를 반환
	if (S_OK != D3DReflect(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&CompileInfo)))
	{
		MsgAssert("쉐이더 리플렉션에 실패했습니다.");
		return;
	}
	// <<설명>>
	/*1. 컴파일된 HLSL 셰이더의 시작 주소*/
	/*2. 바이트코드의 크기*/
	/*3. 리플렉션 인터페이스 ID(직접 __uuidof(ID3D11ShaderReflection) 써도 됨)*/
	/*4. 결과로 생성될 리플렉션 인터페이스 포인터*/

	D3D11_SHADER_DESC Info;
	CompileInfo->GetDesc(&Info); // 이 셰이더가 몇 개의 상수버퍼, 텍스처, 샘플러를 쓰는지 등의 총괄 정보를 Info 구조체에 채움
	D3D11_SHADER_INPUT_BIND_DESC ResDesc;

	// 바인딩 슬롯 갯수만큼 반복
	for (UINT i = 0; i < Info.BoundResources; i++)
	{
		CompileInfo->GetResourceBindingDesc(i, &ResDesc); // i번째 리소스의 이름, 바인딩 슬롯, 타입 등을 ResDesc에 채움
		D3D_SHADER_INPUT_TYPE Type = ResDesc.Type; // 리소스 타입 파싱
		std::string Name = ResDesc.Name; // 바인딩 슬롯 원본 이름
		std::string UpperName = Base_String::ToUpper(ResDesc.Name); // 바인딩 슬롯 대문자

		switch (Type)
		{
		case D3D_SIT_CBUFFER:
		{
			ID3D11ShaderReflectionConstantBuffer* CBufferPtr = CompileInfo->GetConstantBufferByName(ResDesc.Name); // ID3D11ShaderReflectionConstantBuffer 획득

			D3D11_SHADER_BUFFER_DESC BufferDesc; // CBuffer 메타 정보 획득
			CBufferPtr->GetDesc(&BufferDesc);

			std::shared_ptr<Ext_DirectXConstantBuffer> ConstantBuffer = Ext_DirectXConstantBuffer::CreateConstantBuffer(UpperName, BufferDesc, BufferDesc.Size); // 상수버퍼 클래스 생성

			// 상수버퍼 세터 데이터 입력 후 저장
			ConstantBufferSetter Set; 
			Set.OwnerShader = GetSharedFromThis<Ext_DirectXShader>(); // 현재 셰이더
			Set.Name = UpperName; // 이름 저장
			Set.BindPoint = ResDesc.BindPoint; // GPU에 바인딩할 슬롯 정보 register(b0)
			Set.ConstantBuffer = ConstantBuffer; // 상수 버퍼 저장

			BufferSetter.InsertConstantBufferSetter(Set); // 세터에 저장

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
				// 기본 텍스쳐는 DefaultTex.png로 설정
				std::shared_ptr<Ext_DirectXTexture> TextureResource = Ext_DirectXTexture::Find("DefaultTex.png");

				TextureSetter Setter;
				Setter.OwnerShader = GetSharedFromThis<Ext_DirectXShader>();
				Setter.Name = UpperName;
				Setter.BindPoint = ResDesc.BindPoint; // GPU에 바인딩할 슬롯 정보 register(t0)
				Setter.Texture = TextureResource;

				BufferSetter.InsertTextureSetter(Setter);
			}

			break;
		}
		case D3D_SIT_SAMPLER:
		{
			// 바인딩 슬롯 이름과 동일한 샘플러를 찾지 못하면, 기본 Base 샘플러로 변경
			std::shared_ptr<Ext_DirectXSampler> SamplerResource = Ext_DirectXSampler::Find(UpperName);
			if (nullptr == SamplerResource) SamplerResource = Ext_DirectXSampler::Find("LinearClampSampler");

			// 샘플러 세터 데이터 입력 후 저장
			SamplerSetter Set;
			Set.OwnerShader = GetSharedFromThis<Ext_DirectXShader>();
			Set.Name = UpperName;
			Set.BindPoint = ResDesc.BindPoint; // GPU에 바인딩할 슬롯 정보 register(s0)
			Set.Sampler = SamplerResource; // 샘플러 저장

			BufferSetter.InsertSamplerSetter(Set);

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