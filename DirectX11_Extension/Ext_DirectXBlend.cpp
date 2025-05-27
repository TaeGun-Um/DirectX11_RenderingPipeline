#include "PrecompileHeader.h"
#include "Ext_DirectXBlend.h"
#include "Ext_DirectXDevice.h"

void Ext_DirectXBlend::Setting()
{
	if (nullptr == BlendState)
	{
		MsgAssert("생성되지 않은 레스터라이저 스테이트 입니다.");
	}

	if (BlendInfo.RenderTarget[0].SrcBlend == D3D11_BLEND_BLEND_FACTOR)
	{
		MsgAssert("아직 처리 하지 않는 블랜드 방식입니다 블랜드 필터 입력기능을 만들어 주세요.");
	}

	Ext_DirectXDevice::GetContext()->OMSetBlendState(BlendState.Get(), nullptr, Mask);
}

void Ext_DirectXBlend::CreateBlend(const D3D11_BLEND_DESC& _Value)
{
	BlendInfo = _Value;

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateBlendState(&BlendInfo, BlendState.GetAddressOf()))
	{
		MsgAssert("블랜드 스테이트 생성에 실패했습니다.");
	}
}