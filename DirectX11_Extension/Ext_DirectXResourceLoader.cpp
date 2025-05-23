#include "PrecompileHeader.h"
#include "Ext_DirectXResourceLoader.h"

#include <DirectX11_Base/Base_Directory.h>

#include "Ext_DirectXVertexData.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_DirectXMesh.h"
#include "Ext_DirectXShader.h"
#include "Ext_DirectXMaterial.h"

// DirectX�� �ʿ��� ���ҽ��� �ε�
void Ext_DirectXResourceLoader::Initialize()
{
	MakeVertex(); // Ext_DirectXVertexData Ŭ������ InputLayoutElement�� SemanticName, Format ����
	MakeSampler();
	MakeBlend();
	MakeDepth();
	MakeRasterizer();
	ShaderCompile(); // ���̴� ������ �� ������� ����
	MakeMaterial(); // ��Ƽ����(����������) ����
}

// ���� ���� ����(InputLayout, VertexBuffer, IndexBuffer)
void Ext_DirectXResourceLoader::MakeVertex() 
{
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	// Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
	// Ext_DirectXVertexData::GetInputLayoutData().AddInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);

	// �ﰢ��
	{
		std::vector<Ext_DirectXVertexData> Vertices;
		Vertices.resize(3);

		Vertices[0] = { { 0.0f, 0.5f, 0.0 }, { 1, 0, 0, 1 } };
		Vertices[1] = { { 0.5f, -0.5f, 0.0f }, { 0, 1, 0, 1 } };
		Vertices[2] = { { -0.5f, -0.5f, 0.0f }, { 0, 0, 1, 1 } };

		std::vector<UINT> ArrIndex = { 0, 1, 2 };

		Ext_DirectXVertexBuffer::CreateVertexBuffer("Triangle", Vertices);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("Triangle", ArrIndex);
		Ext_DirectXMesh::CreateMesh("Triangle");
	}

	// �⺻ �޽� ����, Rect
	{
		std::vector<Ext_DirectXVertexData> ArrVertex;
		ArrVertex.resize(4);

		ArrVertex[0] = { { -0.5f,  0.5f, 0.0f, 1.0f }, { 1, 0, 0, 1 }, /*{ 0.0f, 0.0f }*/ };
		ArrVertex[1] = { {  0.5f,  0.5f, 0.0f, 1.0f }, { 0, 1, 0, 1 }, /*{ 1.0f, 0.0f }*/ };
		ArrVertex[2] = { { -0.5f, -0.5f, 0.0f, 1.0f }, { 0, 0, 1, 1 }, /*{ 1.0f, 1.0f }*/ };
		ArrVertex[3] = { {  0.5f, -0.5f, 0.0f, 1.0f }, { 1, 1, 0, 1 }, /*{ 0.0f, 1.0f }*/ };

		std::vector<UINT> ArrIndex = { 0, 1, 2, 2, 1, 3 };

		Ext_DirectXVertexBuffer::CreateVertexBuffer("Rect", ArrVertex);
		Ext_DirectXIndexBuffer::CreateIndexBuffer("Rect", ArrIndex);
		Ext_DirectXMesh::CreateMesh("Rect");
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

// ������ ���������� ����
void Ext_DirectXResourceLoader::MakeMaterial()
{
	std::shared_ptr<Ext_DirectXMaterial> NewRenderingPipeline = Ext_DirectXMaterial::CreateMaterial("Basic");
	NewRenderingPipeline->SetVertexShader("Basic_VS");
	NewRenderingPipeline->SetPixelShader("Basic_PS");
	// NewRenderingPipeline->SetBlendState("BaseBlend");
	// NewRenderingPipeline->SetDepthState("EngineDepth");
	// NewRenderingPipeline->SetRasterizer("Engine2DBase");
}
