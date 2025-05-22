#include "PrecompileHeader.h"
#include "Ext_DirectXResourceLoader.h"

#include <DirectX11_Base/Base_Directory.h>

#include "Ext_DirectXInputLayout.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXShader.h"

#include "Ext_DirectXDevice.h" // 임시
#include "Ext_DirectXVertexShader.h" // 임시
#include <d3dcompiler.h> // 임시
#include <D3D11Shader.h>  // 임시
#pragma comment(lib, "d3d11.lib")  // 임시
#pragma comment(lib, "d3dcompiler")  // 임시
#pragma comment(lib, "dxguid")  // 임시
#pragma comment(lib, "DXGI")  // 임시

COMPTR<ID3D11InputLayout> Ext_DirectXResourceLoader::InputLayOut; // 임시

// DirectX에 필요한 리소스를 로드
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

// 정점 정보 생성(InputLayout, VertexBuffer, IndexBuffer)
void Ext_DirectXResourceLoader::MakeVertex() 
{
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	// Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
	// Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);

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

// 셰이더 정보 생성(Shader 오토컴파일, ConstantBuffer)
void Ext_DirectXResourceLoader::ShaderCompile()
{
	// 셰이더 생성 규칙 : [1], [2]를 모두 만족해야 정상 컴파일 진행
	// [1] 이름 + "_" + "Type" == Basic_PS
	// [2] 내부 main(EntryPoint) 이름도 동일하게 설정
	Base_Directory Dir;
	Dir.MakePath("../Shader");
	std::vector<std::string> Paths = Dir.GetAllFile({ "hlsl" });
	for (const std::string& ShaderPath : Paths)
	{
		std::string EntryPoint = Dir.FindEntryPoint(ShaderPath);
		Ext_DirectXShader::ShaderAutoCompile(ShaderPath, EntryPoint.c_str());
	}

	// CreateInputLayout은 정점 버퍼 구조와 셰이더 입력 구조 간의 매핑을 정의
	// 일단 생성해야되서 적음, 구조 잡으면서 나중에 옮길 예정

	// Ext_DirectXVertexShader
	std::shared_ptr<Ext_DirectXVertexShader> VS = Ext_DirectXVertexShader::Find("Basic_VS");

	Ext_DirectXDevice::GetDevice()->CreateInputLayout
	(
		Ext_DirectXInputLayout::GetInputLayoutData().GetInputLayoutDescs().data(),
		static_cast<UINT>(Ext_DirectXInputLayout::GetInputLayoutData().GetInputLayoutDescs().size()),
		VS->GetBinaryCode()->GetBufferPointer(),
		VS->GetBinaryCode()->GetBufferSize(),
		InputLayOut.GetAddressOf()
	);
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
