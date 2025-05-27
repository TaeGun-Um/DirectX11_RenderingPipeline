#include "PrecompileHeader.h"
#include "Ext_DirectXRasterizer.h"
#include "Ext_DirectXDevice.h"

// 재할당 전에는 해제 하고 할당
void Ext_DirectXRasterizer::Release()
{
	SolidState.Reset();
	WireframeState.Reset();
}

// 레스터라이저 세팅, CurState는 솔리드라서 솔리드 레스터라이저로 세팅됨
void Ext_DirectXRasterizer::Setting()
{
	if (nullptr == CurState)
	{
		MsgAssert("생성되지 않은 레스터라이저 스테이트 입니다.");
	}

	Ext_DirectXDevice::GetContext()->RSSetState(CurState);
}

// ID3D11RasterizerState 생성, 와이어프레임과 솔리드 상태 모두 미리 생성함, 설정 상태는 D3D11_FILL_SOLID임
void Ext_DirectXRasterizer::CreateRasterizer(const D3D11_RASTERIZER_DESC& _Value)
{
	Release();
	RaterizerInfo = _Value;

	RaterizerInfo.FillMode = D3D11_FILL_WIREFRAME;
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateRasterizerState(&RaterizerInfo, &WireframeState))
	{
		MsgAssert("와이어 프레임 레스터라이저 스테이트 생성에 실패했습니다.");
	}

	RaterizerInfo.FillMode = D3D11_FILL_SOLID;
	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateRasterizerState(&RaterizerInfo, &SolidState))
	{
		MsgAssert("솔리드 레스터라이저 스테이트 생성에 실패했습니다.");
	}

	// 생성된 이후에는 언제든지 이 함수를 호출해서 세팅 가능(솔리드와 와이어)
	SetFILL_MODE(D3D11_FILL_SOLID);
}