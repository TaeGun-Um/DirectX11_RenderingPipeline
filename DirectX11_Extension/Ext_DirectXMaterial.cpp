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
		MsgAssert("�������� �ʴ� ���ؽ����̴��� ������ �� ����");
		return;
	}
}

void Ext_DirectXMaterial::SetPixelShader(std::string_view _Name)
{
	std::string UpperName = Base_String::ToUpper(_Name);
	PixelShader = Ext_DirectXPixelShader::Find(UpperName);

	if (nullptr == PixelShader)
	{
		MsgAssert("�������� �ʴ� �ȼ� ���̴��� ������ �� ����");
	}
}

void Ext_DirectXMaterial::SetBlendState(std::string_view _Name)
{
	//std::string UpperName = Base_String::ToUpper(_Name);
	//BlendState = Ext_DirectXBlend::Find(UpperName);

	//if (nullptr == BlendState)
	//{
	//	MsgAssert("�������� �ʴ� ���带 �����Ϸ��� �߽��ϴ�.");
	//	return;
	//}
}

void Ext_DirectXMaterial::SetDepthState(std::string_view _Name)
{
	//std::string UpperName = Base_String::ToUpper(_Name);
	//DepthState = Ext_DirectXDepthState::Find(UpperName);

	//if (nullptr == DepthState)
	//{
	//	MsgAssert("�������� �ʴ� ���̹��� ������Ʈ�� �����Ϸ��� �߽��ϴ�.");
	//	return;
	//}
}

void Ext_DirectXMaterial::SetRasterizer(std::string_view _Name)
{
	//std::string UpperName = Base_String::ToUpper(_Name);
	//Rasterizer = Ext_DirectXRasterizer::Find(UpperName);

	//if (nullptr == Rasterizer)
	//{
	//	MsgAssert("�������� �ʴ� �����Ͷ������� ����Ϸ��� �߽��ϴ�.");
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
		MsgAssert("���ؽ� ���̴��� �������� �ʾƼ� ���ؽ� ���̴� ������ ������ �� �����ϴ�.");
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
	//	MsgAssert("�����Ͷ������� �������� �ʾƼ� ������ �Ұ����մϴ�.");
	//	return;
	//}

	//Rasterizer->SetFILL_MODE(FILL_MODE);
	//Rasterizer->Setting();
}

void Ext_DirectXMaterial::PixelShaderSetting()
{
	if (nullptr == PixelShader)
	{
		MsgAssert("�ȼ� ���̴��� �������� �ʾƼ� �ȼ� ���̴� ������ ������ �� �����ϴ�.");
		return;
	}

	PixelShader->PixelShaderSetting();
}

void Ext_DirectXMaterial::OutputMergerSetting()
{
	//if (nullptr == BlendState)
	//{
	//	MsgAssert("���尡 �������� �ʾ� �ƿ�ǲ ���� ������ �Ϸ��Ҽ��� �����ϴ�.");
	//	return;
	//}

	//BlendStatePtr->Setting();

	//if (nullptr == DepthState)
	//{
	//	MsgAssert("���尡 �������� �ʾ� �ƿ�ǲ ���� ������ �Ϸ��Ҽ��� �����ϴ�.");
	//	return;
	//}

	//DepthState->Setting();
}