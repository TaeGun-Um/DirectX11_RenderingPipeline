#include "PrecompileHeader.h"
#include "Ext_DirectXSampler.h"
#include <DirectX11_Base/Base_Debug.h>
#include "Ext_DirectXDevice.h"

// 샘플러 생성
void Ext_DirectXSampler::CreateSampler(const D3D11_SAMPLER_DESC& _Desc)
{
	if (nullptr != SmplerState)
	{
		SmplerState.Reset(); // 기존 샘플러 있을 경우, 해제한 뒤 다시 사용
	}

	SamplerInfo = _Desc;

	// GPU에 실제 샘플러 상태 객체 생성, SmplerState에 저장
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateSamplerState(&SamplerInfo, SmplerState.GetAddressOf()))
	{
		MsgAssert("샘플러 생성에 실패했습니다.");
		return;
	}
}

// 정점 셰이더에 슬롯별로 샘플러 바인딩, 정점 셰이더 샘플러 바인딩은 거의 사용하지 않고 픽셀 셰이더에서만 일어난다.
void Ext_DirectXSampler::VSSetting(UINT _Slot)
{
	Ext_DirectXDevice::GetContext()->VSSetSamplers(_Slot, 1, SmplerState.GetAddressOf());
}

// 픽셀 셰이더에 슬롯별로 샘플러 바인딩(register(b0)에 바인딩됨) -> Tex.Sample(Sampler, uv)에 사용
void Ext_DirectXSampler::PSSetting(UINT _Slot)
{
	Ext_DirectXDevice::GetContext()->PSSetSamplers(_Slot, 1, SmplerState.GetAddressOf());
}