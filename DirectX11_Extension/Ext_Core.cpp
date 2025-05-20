#include "PrecompileHeader.h"
#include "Ext_Core.h"
#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Debug.h>
#include "Ext_DirectXDevice.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_DirectXResourceLoader.h"

#include "Ext_DirectXVertex.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"

void Ext_Core::Run(HINSTANCE _hInstance, const float4& _ScreenSize, bool _IsFullScreen)
{
	Base_Debug::LeakCheck();
	int* TrustLeak = new int;

	// 윈도우 창 생성 후 루프문 시작
	Base_Windows::WindowCreate(_hInstance, _ScreenSize, _IsFullScreen);
	Base_Windows::WindowLoop(Ext_Core::Start, Ext_Core::Update, Ext_Core::End);
}

void Ext_Core::Start()
{
	// After Create Window, EngineStart
	Ext_DirectXDevice::Initialize(); // 디바이스, 컨텍스트, 스왑체인, 렌더타겟 생성
	Ext_DirectXResourceLoader::Initialize(); // DirectX에 활용할 리소스 생성
}

void Ext_Core::Update()
{
	// After EngineStart, EngineLoop
	RenderTest();
}

void Ext_Core::End()
{
	// After Window Destroy, Process ending
	int EndVal = 0;
}

void Ext_Core::RenderTest()
{
	// 1. 메인 렌더 타겟 가져오기
	std::shared_ptr<Ext_DirectXRenderTarget> MainRenderTarget = Ext_DirectXDevice::GetMainRenderTarget();

	// 2. 렌더 타겟 및 뷰포트 바인딩
	COMPTR<ID3D11RenderTargetView> RTV = MainRenderTarget->GetTexture(0)->GetRTV();
	COMPTR<ID3D11DepthStencilView> DSV = MainRenderTarget->GetDepthTexture()->GetDSV();
	D3D11_VIEWPORT* ViewPort = MainRenderTarget->GetViewPort(0);

	Ext_DirectXDevice::GetContext()->OMSetRenderTargets(1, RTV.GetAddressOf(), DSV.Get());
	Ext_DirectXDevice::GetContext()->RSSetViewports(1, ViewPort);

	// 3. 렌더 타겟 및 뎁스 클리어
	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; // 파란색
	Ext_DirectXDevice::GetContext()->ClearRenderTargetView(RTV.Get(), ClearColor);
	Ext_DirectXDevice::GetContext()->ClearDepthStencilView(DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 4. ==========렌더링==============
	std::shared_ptr<Ext_DirectXVertexBuffer> VB = Ext_DirectXVertexBuffer::Find("Triangle");
	COMPTR<ID3D11Buffer>& TriangleVertexBuffer = VB->GetBuffer();  // 직접 getter 필요 시 만들어야 함

	UINT Size = sizeof(TriangleVertexBuffer);
	UINT Offset = 0;
	Ext_DirectXDevice::GetContext()->IASetVertexBuffers(0, 1, TriangleVertexBuffer.GetAddressOf(), &Size, &Offset); // vertexBuffer는 전역/멤버 등으로 생성해 둔 상태여야 함



	// 4-2. 입력 레이아웃 및 프리미티브 타입 설정
	Ext_DirectXDevice::GetContext()->IASetInputLayout(Ext_DirectXResourceLoader::GetInputLayout().Get()); // 생성된 input layout
	Ext_DirectXDevice::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 4-3. 셰이더 바인딩
	Ext_DirectXDevice::GetContext()->VSSetShader(Ext_DirectXResourceLoader::GetVertexShader(), nullptr, 0);
	Ext_DirectXDevice::GetContext()->PSSetShader(Ext_DirectXResourceLoader::GetPixelShader(), nullptr, 0);

	// 4-4. 실제 드로우
	Ext_DirectXDevice::GetContext()->Draw(3, 0);
	// ==========렌더링 끝==============

	// 5. 화면 출력
	Ext_DirectXDevice::GetSwapChain()->Present(1, 0);
}

