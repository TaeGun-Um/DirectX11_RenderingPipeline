#include "PrecompileHeader.h"
#include "Ext_DirectXDepth.h"
#include "Ext_DirectXDevice.h"

void Ext_DirectXDepth::Setting()
{
	if (nullptr == DSS)
	{
		MsgAssert("깊이버퍼 스테이트가 만들어지지 않았습니다.");
	}

	Ext_DirectXDevice::GetContext()->OMSetDepthStencilState(DSS.Get(), 0);
}

void Ext_DirectXDepth::CreateDepth(const D3D11_DEPTH_STENCIL_DESC& _Value)
{
	Desc = _Value;

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateDepthStencilState(&Desc, DSS.GetAddressOf()))
	{
		MsgAssert("깊이 버퍼 스테이트 생성에 실패했습니다.");
	}
}