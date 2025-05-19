#include "PrecompileHeader.h"
#include "Ext_DirectXResourceLoader.h"
#include "Ext_DirectXVertex.h"

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
	Ext_DirectXVertex::GetInputLayoutElement().AddInputLayoutElement("TEXCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertex::GetInputLayoutElement().AddInputLayoutElement("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Ext_DirectXVertex::GetInputLayoutElement().AddInputLayoutElement("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT);
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

}

void Ext_DirectXResourceLoader::MakeRasterizer() 
{

}

void Ext_DirectXResourceLoader::MakeMaterial()
{

}
