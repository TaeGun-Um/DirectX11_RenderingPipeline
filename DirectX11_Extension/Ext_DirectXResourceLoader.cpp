#include "PrecompileHeader.h"
#include "Ext_DirectXResourceLoader.h"
#include "Ext_DirectXVertex.h"
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
	MakeVertex(); // Ext_DirectXVertex 클래스의 InputLayoutElement에 SemanticName, Format 결정
	MakeSampler();
	MakeBlend();
	MakeDepth();
	ShaderCompile();
	MakeRasterizer();
	MakeMaterial();
}

void Ext_DirectXResourceLoader::MakeVertex() 
{
	Ext_DirectXVertex::GetInputLayoutElement().AddInputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertex::GetInputLayoutElement().AddInputLayoutElement("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertex::GetInputLayoutElement().AddInputLayoutElement("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertex::GetInputLayoutElement().AddInputLayoutElement("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);

	// { DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT4(1, 0, 0, 1) }, // 상단 (빨강)
	// { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0, 1, 0, 1) }, // 우하단 (초록)
	// { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f),DirectX::XMFLOAT4(0, 0, 1, 1) }  // 좌하단 (파랑)
	std::vector<Ext_DirectXVertex> Vertices;
	Vertices.resize(3);

	Vertices[0].GetVertexData() = { { 0.0f, 0.5f, 0.0 }, { 1, 0, 0, 1 } };
	Vertices[1].GetVertexData() = { { 0.5f, -0.5f, 0.0f }, { 0, 1, 0, 1 } };
	Vertices[2].GetVertexData() = { { -0.5f, -0.5f, 0.0f }, { 0, 0, 1, 1 } };

	Ext_DirectXVertexBuffer::CreateVertexBuffer("Triangle", Vertices);
	Ext_DirectXIndexBuffer::CreateIndexBuffer("Triangle", Vertices);
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
	COMPTR<ID3DBlob> ErrorBlob = nullptr;
	COMPTR<ID3DBlob> VSBlob = nullptr;

	unsigned int Flag = 0;
#ifdef _DEBUG
	Flag = D3D10_SHADER_DEBUG;
#endif
	Flag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	HRESULT hr = D3DCompileFromFile(L"../Shader/BaseVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", Flag, 0, VSBlob.GetAddressOf(), ErrorBlob.GetAddressOf());

	if (FAILED(hr))
	{
		std::stringstream ss;
		ss << "D3DCompileFromFile failed. HRESULT = 0x" << std::hex << hr << std::endl;
		OutputDebugStringA(ss.str().c_str());
	}

	//if (S_OK != )
	//{
	//	MsgAssert("VertexShader 컴파일 실패");
	//	return;
	//}

	Ext_DirectXDevice::GetDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &BaseVertexShader);

	COMPTR<ID3DBlob> PSBlob = nullptr;
	if (S_OK != D3DCompileFromFile(L"../Shader/BasePixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", Flag, 0, PSBlob.GetAddressOf(), ErrorBlob.GetAddressOf()))
	{
		MsgAssert("PixelShader 컴파일 실패");
		return;
	}

	Ext_DirectXDevice::GetDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &BasePixelShader);

	Ext_DirectXDevice::GetDevice()->CreateInputLayout(
		Ext_DirectXVertex::GetInputLayoutElement().GetInputLayoutDescs().data(),
		static_cast<UINT>(Ext_DirectXVertex::GetInputLayoutElement().GetInputLayoutDescs().size()),
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&InputLayout
	);
}

void Ext_DirectXResourceLoader::MakeRasterizer() 
{

}

void Ext_DirectXResourceLoader::MakeMaterial()
{

}
