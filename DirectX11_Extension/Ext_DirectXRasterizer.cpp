#include "PrecompileHeader.h"
#include "Ext_DirectXRasterizer.h"
#include "Ext_DirectXDevice.h"

void Ext_DirectXRasterizer::Setting()
{
	if (nullptr == CurState)
	{
		MsgAssert("생성되지 않은 레스터라이저 스테이트 입니다.");
	}

	Ext_DirectXDevice::GetContext()->RSSetState(CurState);
}

void Ext_DirectXRasterizer::Release()
{
	SolidState.Reset();
	WireframeState.Reset();
}

void Ext_DirectXRasterizer::CreateRasterizer(const D3D11_RASTERIZER_DESC& _Value)
{
	Release();
	Desc = _Value;

	Desc.FillMode = D3D11_FILL_WIREFRAME;
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateRasterizerState(&Desc, &WireframeState))
	{
		MsgAssert("와이어 프레임 레스터라이저 스테이트 생성에 실패했습니다.");
	}

	Desc.FillMode = D3D11_FILL_SOLID;
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateRasterizerState(&Desc, &SolidState))
	{
		MsgAssert("솔리드 레스터라이저 스테이트 생성에 실패했습니다.");
	}

	SetFILL_MODE(D3D11_FILL_SOLID);
}