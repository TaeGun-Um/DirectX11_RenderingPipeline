#include "PrecompileHeader.h"
#include "Ext_DirectXSampler.h"
#include <DirectX11_Base/Base_Debug.h>
#include "Ext_DirectXDevice.h"

void Ext_DirectXSampler::CreateSampler(const D3D11_SAMPLER_DESC& _Desc)
{
	if (nullptr != SmplerState)
	{
		SmplerState.Reset();
	}

	SamplerInfo = _Desc;

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateSamplerState(&SamplerInfo, SmplerState.GetAddressOf()))
	{
		MsgAssert("샘플러 생성에 실패했습니다." /*+ GetNameToString()*/);
		return;
	}
}

void Ext_DirectXSampler::VSSetting(UINT _Slot)
{
	Ext_DirectXDevice::GetContext()->VSSetSamplers(_Slot, 1, SmplerState.GetAddressOf());
}

void Ext_DirectXSampler::PSSetting(UINT _Slot)
{
	Ext_DirectXDevice::GetContext()->PSSetSamplers(_Slot, 1, SmplerState.GetAddressOf());
}