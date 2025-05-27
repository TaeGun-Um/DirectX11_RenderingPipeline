#include "PrecompileHeader.h"
#include "Ext_DirectXDepth.h"
#include "Ext_DirectXDevice.h"

// DSS를 GPU에 바인딩, 아웃풋머저에 적용
void Ext_DirectXDepth::Setting()
{
	if (nullptr == DSS)
	{
		MsgAssert("깊이버퍼 스테이트가 만들어지지 않았습니다.");
	}

	Ext_DirectXDevice::GetContext()->OMSetDepthStencilState(DSS.Get(), 0);
	/*1. 생성한 DSS 전달*/ 
	/*2. 2번째 인자 0은 스텐실 안쓴다는 뜻*/
}

// DepthStencilState 생성
void Ext_DirectXDepth::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& _Value)
{
	DepthStencilInfo = _Value;

	if (S_OK != Ext_DirectXDevice::GetDevice()->CreateDepthStencilState(&DepthStencilInfo, DSS.GetAddressOf()))
	{
		MsgAssert("깊이 버퍼 스테이트 생성에 실패했습니다.");
	}
	// D3D11_DEPTH_STENCIL_DESC(DSS 생성 설명서)를 바탕으로 DSS 생성
}