#include "PrecompileHeader.h"
#include "Ext_DirectXResourceLoader.h"
#include "Ext_DirectXInputLayout.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"

#include "Ext_DirectXDevice.h"
#include <sstream>
#include <fstream>
COMPTR<ID3D11VertexShader> Ext_DirectXResourceLoader::BaseVertexShader = nullptr;
COMPTR<ID3D11PixelShader> Ext_DirectXResourceLoader::BasePixelShader = nullptr;
COMPTR<ID3D11InputLayout> Ext_DirectXResourceLoader::InputLayout = nullptr;

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

void Ext_DirectXResourceLoader::MakeVertex() 
{
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXInputLayout::GetInputLayoutData().AddInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);

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
	Flag = D3D10_SHADER_DEBUG; // ����� ������ ����
#endif
	Flag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR; // ��켱 ��Ʈ���� ���� ��� ��� (HLSL �� C++ ȣȯ ����)

	// ID3DBlob : ���̴� ����Ʈ�ڵ带 ��� �������̽�
	COMPTR<ID3DBlob> ErrorBlob = nullptr;
	COMPTR<ID3DBlob> VSBlob = nullptr;
	if (S_OK != D3DCompileFromFile(L"../Shader/BaseVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", Flag, 0, VSBlob.GetAddressOf(), ErrorBlob.GetAddressOf()))
	{
		MsgAssert("VertexShader ������ ����");
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

	// �����ϵ� ����Ʈ�ڵ�� GPU�� Vertex Shader ����
	Ext_DirectXDevice::GetDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &BaseVertexShader);

	COMPTR<ID3DBlob> PSBlob = nullptr;
	if (S_OK != D3DCompileFromFile(L"../Shader/BasePixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", Flag, 0, PSBlob.GetAddressOf(), ErrorBlob.GetAddressOf()))
	{
		MsgAssert("PixelShader ������ ����");
		return;
	}

	// �����ϵ� ����Ʈ�ڵ�� GPU�� Pixel Shader ����
	Ext_DirectXDevice::GetDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &BasePixelShader);

	// CreateInputLayout�� ���� ���� ������ ���̴� �Է� ���� ���� ������ ����
	// �ϴ� �����ؾߵǼ� ����, ���� �����鼭 ���߿� �ű� ����
	Ext_DirectXDevice::GetDevice()->CreateInputLayout
	(
		Ext_DirectXInputLayout::GetInputLayoutData().GetInputLayoutDescs().data(),
		static_cast<UINT>(Ext_DirectXInputLayout::GetInputLayoutData().GetInputLayoutDescs().size()),
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&InputLayout
	);

	// �� ������� �ϴ� �̰��� Ŭ�������� �����ߴ�. �ٸ������� �����
	// ���� �����鼭 ���߿� �ű� ����
	// static COMPTR<ID3D11VertexShader> BaseVertexShader;
	// static COMPTR<ID3D11PixelShader> BasePixelShader;
	// static COMPTR<ID3D11InputLayout> InputLayout;
}

void Ext_DirectXResourceLoader::MakeRasterizer() 
{

}

void Ext_DirectXResourceLoader::MakeMaterial()
{

}
