#include "PrecompileHeader.h"
#include "Ext_Core.h"
#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Debug.h>
#include "Ext_DirectXDevice.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_DirectXResourceLoader.h"

void Ext_Core::Run(HINSTANCE _hInstance, const float4& _ScreenSize, bool _IsFullScreen)
{
	Base_Debug::LeakCheck();
	int* TrustLeak = new int;

	// ������ â ���� �� ������ ����
	Base_Windows::WindowCreate(_hInstance, _ScreenSize, _IsFullScreen);
	Base_Windows::WindowLoop(Ext_Core::Start, Ext_Core::Update, Ext_Core::End);
}

void Ext_Core::Start()
{
	// After Create Window, EngineStart
	Ext_DirectXDevice::Initialize(); // ����̽�, ���ؽ�Ʈ, ����ü��, ����Ÿ�� ����
	Ext_DirectXResourceLoader::Initialize(); // DirectX�� Ȱ���� ���ҽ� ����
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
	// 1. ���� ���� Ÿ�� ��������
	std::shared_ptr<Ext_DirectXRenderTarget> MainRenderTarget = Ext_DirectXDevice::GetMainRenderTarget();

	// 2. ���� Ÿ�� �� ����Ʈ ���ε�
	COMPTR<ID3D11RenderTargetView> RTV = MainRenderTarget->GetTexture(0)->GetRTV();
	COMPTR<ID3D11DepthStencilView> DSV = MainRenderTarget->GetDepthTexture()->GetDSV();
	D3D11_VIEWPORT* ViewPort = MainRenderTarget->GetViewPort(0);

	Ext_DirectXDevice::GetContext()->OMSetRenderTargets(1, RTV.GetAddressOf(), DSV.Get());
	Ext_DirectXDevice::GetContext()->RSSetViewports(1, ViewPort);

	// 3. ���� Ÿ�� �� ���� Ŭ����
	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; // �Ķ���
	Ext_DirectXDevice::GetContext()->ClearRenderTargetView(RTV.Get(), ClearColor);
	Ext_DirectXDevice::GetContext()->ClearDepthStencilView(DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 4. ȭ�� ���
	Ext_DirectXDevice::GetSwapChain()->Present(1, 0);
}

