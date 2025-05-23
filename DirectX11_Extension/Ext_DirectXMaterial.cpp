#include "PrecompileHeader.h"
#include "Ext_DirectXMaterial.h"
#include <DirectX11_Base/Base_String.h>

#include "Ext_DirectXVertexShader.h"
#include "Ext_DirectXPixelShader.h"

void Ext_DirectXMaterial::SetVertexShader(std::string_view _Name)
{
	std::string UpperName = Base_String::ToUpper(_Name);
	VertexShader = Ext_DirectXVertexShader::Find(UpperName);

	if (nullptr == VertexShader)
	{
		MsgAssert("존재하지 않는 버텍스셰이더를 세팅할 순 없음");
		return;
	}
}

void Ext_DirectXMaterial::SetPixelShader(std::string_view _Name)
{
	std::string UpperName = Base_String::ToUpper(_Name);
	PixelShader = Ext_DirectXPixelShader::Find(UpperName);

	if (nullptr == PixelShader)
	{
		MsgAssert("존재하지 않는 픽셀 셰이더를 세팅할 순 없음");
	}
}

void Ext_DirectXMaterial::SetBlendState(std::string_view _Name)
{
	//std::string UpperName = Base_String::ToUpper(_Name);
	//BlendState = Ext_DirectXBlend::Find(UpperName);

	//if (nullptr == BlendState)
	//{
	//	MsgAssert("존재하지 않는 블랜드를 세팅하려고 했습니다.");
	//	return;
	//}
}

void Ext_DirectXMaterial::SetDepthState(std::string_view _Name)
{
	//std::string UpperName = Base_String::ToUpper(_Name);
	//DepthState = Ext_DirectXDepthState::Find(UpperName);

	//if (nullptr == DepthState)
	//{
	//	MsgAssert("존재하지 않는 깊이버퍼 스테이트를 세팅하려고 했습니다.");
	//	return;
	//}
}

void Ext_DirectXMaterial::SetRasterizer(std::string_view _Name)
{
	//std::string UpperName = Base_String::ToUpper(_Name);
	//Rasterizer = Ext_DirectXRasterizer::Find(UpperName);

	//if (nullptr == Rasterizer)
	//{
	//	MsgAssert("존재하지 않는 레스터라이저를 사용하려고 했습니다.");
	//}
}

void Ext_DirectXMaterial::MaterialSetting()
{
	VertexShaderSetting();
	// HullShader();
	// Tessellator();
	// DomainShader();
	// GeometryShader();
	// Rasterizer();
	PixelShaderSetting();
	OutputMergerSetting();
}

void Ext_DirectXMaterial::VertexShaderSetting()
{
	if (nullptr == VertexShader)
	{
		MsgAssert("버텍스 쉐이더가 존재하지 않아서 버텍스 쉐이더 과정을 실행할 수 없습니다.");
		return;
	}

	VertexShader->VertexShaderSetting();
}

void Ext_DirectXMaterial::HullShaderSetting()
{

}
void Ext_DirectXMaterial::TessellatorSetting()
{

}
void Ext_DirectXMaterial::DomainShaderSetting()
{

}
void Ext_DirectXMaterial::GeometryShaderSetting()
{
	//if (nullptr == GeometryShader)
	//{
	//	return;
	//}

	//GeometryShader->Setting();
}

void Ext_DirectXMaterial::RasterizerSetting()
{
	//if (nullptr == Rasterizer)
	//{
	//	MsgAssert("레스터라이저가 존재하지 않아서 세팅이 불가능합니다.");
	//	return;
	//}

	//Rasterizer->SetFILL_MODE(FILL_MODE);
	//Rasterizer->Setting();
}

void Ext_DirectXMaterial::PixelShaderSetting()
{
	if (nullptr == PixelShader)
	{
		MsgAssert("픽셀 쉐이더가 존재하지 않아서 픽셀 쉐이더 과정을 실행할 수 없습니다.");
		return;
	}

	PixelShader->PixelShaderSetting();
}

void Ext_DirectXMaterial::OutputMergerSetting()
{
	//if (nullptr == BlendState)
	//{
	//	MsgAssert("블랜드가 존재하지 않아 아웃풋 머저 과정을 완료할수가 없습니다.");
	//	return;
	//}

	//BlendStatePtr->Setting();

	//if (nullptr == DepthState)
	//{
	//	MsgAssert("블랜드가 존재하지 않아 아웃풋 머저 과정을 완료할수가 없습니다.");
	//	return;
	//}

	//DepthState->Setting();
}