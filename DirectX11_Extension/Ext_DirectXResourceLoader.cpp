#include "PrecompileHeader.h"
#include "Ext_DirectXResourceLoader.h"

#include <DirectX11_Base/Base_Directory.h>

#include "Ext_DirectXInputLayout.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXShader.h"

#include "Ext_DirectXDevice.h" // �ӽ�
#include "Ext_DirectXVertexShader.h" // �ӽ�
#include <d3dcompiler.h> // �ӽ�
#include <D3D11Shader.h>  // �ӽ�
#pragma comment(lib, "d3d11.lib")  // �ӽ�
#pragma comment(lib, "d3dcompiler")  // �ӽ�
#pragma comment(lib, "dxguid")  // �ӽ�
#pragma comment(lib, "DXGI")  // �ӽ�

COMPTR<ID3D11InputLayout> Ext_DirectXResourceLoader::InputLayOut; // �ӽ�

// DirectX�� �ʿ��� ���ҽ��� �ε�
void Ext_DirectXResourceLoader::Initialize()
{
	MakeVertex(); // Ext_DirectXInputLayout Ŭ������ InputLayoutElement�� SemanticName, Format ����
	MakeSampler();
	MakeBlend();
	MakeDepth();
	ShaderCompile();
	MakeRasterizer();
	MakeMaterial();
}

// ���� ���� ����(InputLayout, VertexBuffer, IndexBuffer)
void Ext_DirectXResourceLoader::MakeVertex() 
{
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	// Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
	// Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);

	// �ﰢ��
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

	// �⺻ �޽� ����, Rect
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

// ���̴� ���� ����(Shader ����������, ConstantBuffer)
void Ext_DirectXResourceLoader::ShaderCompile()
{
	// ���̴� ���� ��Ģ : [1], [2]�� ��� �����ؾ� ���� ������ ����
	// [1] �̸� + "_" + "Type" == Basic_PS
	// [2] ���� main(EntryPoint) �̸��� �����ϰ� ����
	Base_Directory Dir;
	Dir.MakePath("../Shader");
	std::vector<std::string> Paths = Dir.GetAllFile({ "hlsl" });
	for (const std::string& ShaderPath : Paths)
	{
		std::string EntryPoint = Dir.FindEntryPoint(ShaderPath);
		Ext_DirectXShader::ShaderAutoCompile(ShaderPath, EntryPoint.c_str());
	}

	// CreateInputLayout�� ���� ���� ������ ���̴� �Է� ���� ���� ������ ����
	// �ϴ� �����ؾߵǼ� ����, ���� �����鼭 ���߿� �ű� ����

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
	//	MsgAssert("� ���̴��� ���õ��� �˼����� ������� �Դϴ�.");
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
