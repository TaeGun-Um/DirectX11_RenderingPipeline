#include "PrecompileHeader.h"
#include "Ext_DirectXResourceLoader.h"
#include "Ext_DirectXInputLayout.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"

#include "Ext_DirectXConstantBuffer.h"
#include "Ext_DirectXDevice.h"
#include <sstream>
#include <fstream>
#include <d3dcompiler.h> // 
#include <D3D11Shader.h>
COMPTR<ID3D11VertexShader> Ext_DirectXResourceLoader::BaseVertexShader = nullptr;
COMPTR<ID3D11PixelShader> Ext_DirectXResourceLoader::BasePixelShader = nullptr;
COMPTR<ID3D11InputLayout> Ext_DirectXResourceLoader::InputLayout = nullptr;
std::multimap<std::string, Setter> Ext_DirectXResourceLoader::ConstantBufferSetters;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "DXGI")

void Ext_DirectXResourceLoader::Initialize()
{
	MakeVertex(); // Ext_DirectXInputLayout 클래스의 InputLayoutElement에 SemanticName, Format 결정
	MakeSampler();
	MakeBlend();
	MakeDepth();
	ShaderCompile();
	MakeRasterizer();
	MakeMaterial();
}

void Ext_DirectXResourceLoader::MakeVertex() 
{
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);

	// 삼각형
	{
		std::vector<Ext_DirectXInputLayout> Vertices;
		Vertices.resize(3);

		Vertices[0] = { { 0.0f, 0.5f, 0.0 }, { 1, 0, 0, 1 } };
		Vertices[1] = { { 0.5f, -0.5f, 0.0f }, { 0, 1, 0, 1 } };
		Vertices[2] = { { -0.5f, -0.5f, 0.0f }, { 0, 0, 1, 1 } };

		std::vector<UINT> ArrIndex = { 0, 1, 2 };

		Ext_DirectXVertexBuffer::CreateVertexBuffer("Triangle", Vertices);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("Triangle", ArrIndex);
	}

	// 기본 메쉬 생성, Rect
	{
		std::vector<Ext_DirectXInputLayout> ArrVertex;
		ArrVertex.resize(4);

		ArrVertex[0] = { { -0.5f,  0.5f, 0.0f, 1.0f }, { 1, 0, 0, 1 }, /*{ 0.0f, 0.0f }*/ };
		ArrVertex[1] = { {  0.5f,  0.5f, 0.0f, 1.0f }, { 0, 1, 0, 1 }, /*{ 1.0f, 0.0f }*/ };
		ArrVertex[2] = { { -0.5f, -0.5f, 0.0f, 1.0f }, { 0, 0, 1, 1 }, /*{ 1.0f, 1.0f }*/ };
		ArrVertex[3] = { {  0.5f, -0.5f, 0.0f, 1.0f }, { 1, 1, 0, 1 }, /*{ 0.0f, 1.0f }*/ };

		std::vector<UINT> ArrIndex = { 0, 1, 2, 2, 1, 3 };

		Ext_DirectXVertexBuffer::CreateVertexBuffer("Rect", ArrVertex);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("Rect", ArrIndex);
	}
}

void Ext_DirectXResourceLoader::MakeSampler() 
{

}

void Ext_DirectXResourceLoader::MakeBlend() 
{

}

void Ext_DirectXResourceLoader::MakeDepth() 
{

}

void Ext_DirectXResourceLoader::ShaderCompile() 
{
	unsigned int Flag = 0;

#ifdef _DEBUG
	Flag = D3D10_SHADER_DEBUG; // 디버그 정보를 포함
#endif
	Flag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR; // 행우선 매트릭스 정렬 방식 사용 (HLSL ↔ C++ 호환 용이)

	// ID3DBlob : 셰이더 바이트코드를 담는 인터페이스
	COMPTR<ID3DBlob> ErrorBlob = nullptr;
	COMPTR<ID3DBlob> VSBlob = nullptr;
	if (S_OK != D3DCompileFromFile(L"../Shader/BaseVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", Flag, 0, VSBlob.GetAddressOf(), ErrorBlob.GetAddressOf()))
	{
		MsgAssert("VertexShader 컴파일 실패");
		return;
	}
	// <<설명>>
	/*1. 파일 경로 입력*/
	/*2. 기본 include 처리 방식 설정*/
	/*3. 함수명*/
	/*4. 셰이더 모델, 비주얼 스튜디오 2022에서 기본 생성하면 Vertex Shader Model 5.0이기 떄문에 vs_5_0*/
	/*5. 컴파일 플레그 설정*/
	/*6. 컴파일 플레그 설정*/
	/*7. 출력 바이트코드*/
	/*8. 출력 에러*/

	// 컴파일된 바이트코드로 GPU용 Vertex Shader 생성
	Ext_DirectXDevice::GetDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &BaseVertexShader);

	COMPTR<ID3DBlob> PSBlob = nullptr;
	if (S_OK != D3DCompileFromFile(L"../Shader/BasePixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", Flag, 0, PSBlob.GetAddressOf(), ErrorBlob.GetAddressOf()))
	{
		MsgAssert("PixelShader 컴파일 실패");
		return;
	}

	// 컴파일된 바이트코드로 GPU용 Pixel Shader 생성
	Ext_DirectXDevice::GetDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &BasePixelShader);

	// CreateInputLayout은 정점 버퍼 구조와 셰이더 입력 구조 간의 매핑을 정의
	// 일단 생성해야되서 적음, 구조 잡으면서 나중에 옮길 예정
	Ext_DirectXDevice::GetDevice()->CreateInputLayout
	(
		Ext_DirectXInputLayout::GetInputLayoutData().GetInputLayoutDescs().data(),
		static_cast<UINT>(Ext_DirectXInputLayout::GetInputLayoutData().GetInputLayoutDescs().size()),
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&InputLayout
	);

	// 각 결과물은 일단 이곳의 클래스에서 저장했다. 다른곳에서 써야함
	// 구조 잡으면서 나중에 옮길 예정
	// static COMPTR<ID3D11VertexShader> BaseVertexShader;
	// static COMPTR<ID3D11PixelShader> BasePixelShader;
	// static COMPTR<ID3D11InputLayout> InputLayout;

	// 임시로 상수 버퍼 생성
	ID3D11ShaderReflection* CompileInfo = nullptr;

	// VSBlob, PSBlob : 바이너리코드
	{
		if (S_OK != D3DReflect(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&CompileInfo)))
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
		if (S_OK != D3DReflect(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&CompileInfo)))
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

	//Set.Res->VS
}

void Ext_DirectXResourceLoader::MakeRasterizer() 
{

}

void Ext_DirectXResourceLoader::MakeMaterial()
{

}

void Setter::Setting()
{
	//Res->ChangeData(CPUData, CPUDataSize);

	//// ShaderType Type = ParentShader->GetType();

	//switch (Type)
	//{
	//case ShaderType::None:
	//{
	//	MsgAssert("어떤 쉐이더에 세팅될지 알수없는 상수버퍼 입니다.");
	//	break;
	//}
	//case ShaderType::Vertex:
	//{
	//	Res->VSSetting(BindPoint);
	//	break;
	//}
	//case ShaderType::Pixel:
	//{
	//	Res->PSSetting(BindPoint);
	//	break;
	//}
	//default:
	//	break;
	//}
}
