#include "PrecompileHeader.h"
#include "Ext_DirectXDevice.h"
#include <DirectX11_Base/Base_Debug.h>
#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Math.h>
#include "Ext_DirectXTexture.h"
#include "Ext_DirectXRenderTarget.h"

std::shared_ptr<Ext_DirectXRenderTarget> Ext_DirectXDevice::MainRenderTarget = nullptr;
COMPTR<ID3D11Device> Ext_DirectXDevice::Device = nullptr;
COMPTR<ID3D11DeviceContext> Ext_DirectXDevice::Context = nullptr;
COMPTR<IDXGISwapChain> Ext_DirectXDevice::SwapChain = nullptr;

// DirectX11 ����
void Ext_DirectXDevice::Initialize()
{
	if (!Base_Windows::GetHWnd())
	{
		MsgAssert("������ �ʱ�ȭ ������ Initialize�� ������ �� �����ϴ�.");
		return;
	}
	
	int iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG; // DirectX 11 ����� ��� ����

#endif
	D3D_FEATURE_LEVEL Level = D3D_FEATURE_LEVEL_11_0;
	COMPTR<IDXGIAdapter> Adapter = GetHighPerformanceAdapter();
	if (!Adapter)
	{
		MsgAssert("�׷���ī�� ����� �������̽� ȹ�� ����");
		return;
	}

	// ����̽�, ����̽� ���ؽ�Ʈ ���� �Լ�
	HRESULT Hr = D3D11CreateDevice
	(
		/*1*/Adapter.Get(),
		/*2*/D3D_DRIVER_TYPE_UNKNOWN,
		/*3*/nullptr,
		/*4*/iFlag,
		/*5*/nullptr,
		/*6*/0,
		/*7*/D3D11_SDK_VERSION,
		/*8*/Device.GetAddressOf(),
		/*9*/&Level,
		/*10*/Context.GetAddressOf()
	);
	// <<����>>
	// [1] IDXGIAdapter*						: ����̽��� ���� �� ����� ���� ����Ϳ� ���� ������, nullptr ���� �� IDXGIFactory1::EnumAdapters�� ���ŵ� ù ��° ������� �⺻���� ������
	// [2] D3D_DRIVER_TYPE				: ����̹� ���� ����, ����͸� ���������� UNKNOWN�� �����ϸ� ��(HARDWARE == GPU ���, REFERENCE == CPU ��� ����Ʈ���� ����, WARP == ���� ����Ʈ���� ����)
	// [3] HMODULE								: ����Ʈ���� �����Ͷ������� �����ϴ� DLL�� ���� �ڵ�, nullptr ���� �� ����Ʈ���� ����̹� ������ ����, ���� nullptr�� �����Ѵ�.
	// [4] UINT									: D3D11_CREATE_DEVICE_FLAG �� ����, ����� ���̾� �÷���
	// [5] const D3D_FEATURE_LEVEL   : ����� ��� ������ ������ �����ϴ� Feature Level �迭�� ���� ������ ����, nullptr ���� �� D3D_FEATURE_LEVEL �迭�� ù ��° ���� ��ȯ��
	// [6] UINT									: [5]�� ���� �迭 ����� ����
	// [7] UINT									: SDK ����, ������ D3D11_SDK_VERSION�� ����
	// [8] ID3D11Device**					: ���� ����̽��� ��ȯ���ִ� ������ ��
	// [9] D3D_FEATURE_LEVEL*			: ������ ������ ����̽��� ��� ���� ��ȯ, ù ��° D3D_FEATURE_LEVEL ��ȯ����
	// [10] ID3D11DeviceContext**		: ����̽����ؽ�Ʈ ��ȯ���ִ� ������ ��
	
	// Feature Level�� DirectX���� GPU�� �����ϴ� ����� �ܰ躰�� ������ ���̴�. -> �� �׷���ī�尡 �󸶳� �ֽ� ����� �� �� �ִ°�?
	// DirectX11�� ����ϰ� �; GPU�� ��� ���� ���ϸ� �ȵǱ⶧����, �̰ɷ� �޾Ƽ� Ȯ���غ��� �ȴ�.

	if (S_OK != Hr)
	{
		MsgAssert("����̽� ������ �����߽��ϴ�.");
		return;
	}

	// ����� �ٽ����� ����
	if (nullptr != Adapter)
	{
		Adapter.Reset();
	}

	// DirectX11�� �ʱ�ȭ�Ȱ� �ƴ϶�� �ش� ����ʹ� DirectX11�� �������� �ʴ� �׷���ī����
	if (Level != D3D_FEATURE_LEVEL_11_0)
	{
		MsgAssert("���̷�Ʈ 11�� �������� �ʴ� �׷���ī�� �Դϴ�");
		return;
	}

	Hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED); // ��Ƽ������ ��� ����
	CreateSwapChain(); // ����ü�� ����
}

// �׷���ī�� ���� ��������
COMPTR<IDXGIAdapter> Ext_DirectXDevice::GetHighPerformanceAdapter()
{
	COMPTR<IDXGIFactory> Factory = nullptr;
	COMPTR<IDXGIAdapter> Adapter = nullptr;

	// ���丮 ����
	HRESULT HR = CreateDXGIFactory
	(
		__uuidof(IDXGIFactory),				// [1] ������ �������̽��� GUID�� ����, IDXGIFactory�� DXGI ���丮 �������̽��̸� __uuidof�� ������ Ÿ�ӿ� GUID�� �����ϴ� ��ũ��
		(void**)Factory.GetAddressOf()	// [2] ����� ������ IDXGIFactory ��ü�� ���� �����͸� ����
	);

	if (nullptr == Factory)
	{
		MsgAssert("�׷���ī�忡�� ���丮 �������̽��� �������� ���߽��ϴ�.");
		return COMPTR<IDXGIAdapter>();
	}

	// ���� GPU �߿���, "���� ���� ���� ���� �޸�(DedicatedVideoMemory)"�� ���� ����͸� ����
	size_t prevAdapterVideoMemory = 0;
	for (UINT Adapterindex = 0; ; Adapterindex++)
	{
		COMPTR<IDXGIAdapter> CurAdapter = nullptr;
		Factory->EnumAdapters(Adapterindex, CurAdapter.GetAddressOf()); // DXGI ���丮�� ���� ����͸� �ϳ� ����

		if (nullptr == CurAdapter) // ����Ͱ� �� �̻� ������ ���� ����
		{
			break;
		}

		DXGI_ADAPTER_DESC Desc;
		CurAdapter->GetDesc(&Desc); // ���� ������� ������ DXGI_ADAPTER_DESC�� ä��

		if (prevAdapterVideoMemory <= Desc.DedicatedVideoMemory) // ������ ���õ� ����ͺ��� �޸𸮰� ���ų� ���ٸ� ����
		{
			prevAdapterVideoMemory = Desc.DedicatedVideoMemory;

			if (nullptr != Adapter)
			{
				Adapter.Reset(); // ���� ����� �޸� ����
			}

			Adapter = CurAdapter; // ���� ����͸� ���� �ĺ��� ����
			continue;
		}

		CurAdapter.Reset(); // ��� �� �ݵ�� Release
	}

	Factory.Reset(); // ��� �� �ݵ�� Release ���ֵ��� �Ѵ�.

	return Adapter;
}

// ����ü�� ����
void Ext_DirectXDevice::CreateSwapChain()
{
	float4 ScreenSize = Base_Windows::GetScreenSize();
	DXGI_SWAP_CHAIN_DESC SwapChainDesc = { 0, }; // DXGI_SWAP_CHAIN_DESC ����ü ����

	/*1*/SwapChainDesc.BufferCount = 2;
	/*2*/SwapChainDesc.BufferDesc.Width = ScreenSize.uix();
	/*3*/SwapChainDesc.BufferDesc.Height = ScreenSize.uiy();
	/*4*/SwapChainDesc.OutputWindow = Base_Windows::GetHWnd();
	/*5*/SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	/*6*/SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	/*7*/SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	/*8*/SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	/*9*/SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	/*10*/SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	/*11*/SwapChainDesc.SampleDesc.Quality = 0;
	/*12*/SwapChainDesc.SampleDesc.Count = 1;
	/*13*/SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	/*14*/SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	/*15*/SwapChainDesc.Windowed = true;
	// <<����>>
	// [1] : ���� ���۸� ���, GPU�� �ϳ��� ���ۿ� �������ϴ� ���� �ٸ� �ϳ��� ȭ�鿡 ǥ���Ͽ� �����Ӱ� Ƽ� ����
	// [2] : ����ü�� ���� �ػ� ����(�ʺ�)
	// [3] : ����ü�� ���� �ػ� ����(����)
	// [4] : ������ ����� ����� �������� �ڵ� ����, CreateWindow()�� ���� â�� �ڵ�(���� �����ϴ� ���μ��� �ڵ� �־���)
	// [5] : ȭ�� �ֻ��� ����, ���⼭ 1
	// [6] : ȭ�� �ֻ��� ����, ���⼭ 60, �̷��� Numerator/ Denominator�� �Ǿ� 60 / 1 = 60Hz�� ������
	// [7] : �ȼ� ���� ����, DXGI_FORMAT_R8G8B8A8_UNORM�� �� RGBA ä���� 8bit�̸� 0~255 ������ ����ȭ �� ���� ����
	// [8] : ��ĵ���� ������ ������� �ʴ´ٴ� ������ �⺻ �����϶�� ����
	// [9] : ȭ�� Ȯ��/��� ��� ����, DXGI_MODE_SCALING_UNSPECIFIED�� ���÷��� �⺻ ������ �����ٴ� ���̴�.
	// [10] : �ش� ���� ü���� ���� Ÿ������ ����ϰ�, ���̴��� �ش� �ɼ��� ����ϵ��� ����
	// [11] : Count�� 1�̸� ��Ƽ�ٸ���� ��� ���Ѵٴ� ��
	// [12] : ��Ƽ ���ø�(MSAA) ��Ȱ��ȭ
	// [13] : �ֽ� ����� �ø� ü�� ����Ѵٴ� ��
	// [14] : ��üȭ�� ��ȯ �� �ػ� ������ ����� ������, �ش� ������Ʈ�� �Ⱦ����� ����
	// [15] : â��� ����(false�� ��üȭ�� ���)

	COMPTR<IDXGIDevice> SwapDevice = nullptr;
	COMPTR<IDXGIAdapter> SwapAdapter = nullptr;
	COMPTR<IDXGIFactory> SwapFactory = nullptr;

	// ����̽� �����������̽����� SwapDevice �� ����
	Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(SwapDevice.GetAddressOf()));
	if (nullptr == SwapDevice)
	{
		MsgAssert("DXGI ����̽��� DirectX����̽����� ������ ���߽��ϴ�.");
		return;
	}

	// SwapDevice���� SwapAdapter �� ����
	SwapDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(SwapAdapter.GetAddressOf()));
	if (nullptr == SwapAdapter)
	{
		MsgAssert("DXGI ����̽��� DirectX����̽����� ������ ���߽��ϴ�.");
		return;
	}

	// SwapAdapter���� SwapFactory ���� ������ ��, �׷κ��� ����ü���� ������
	SwapAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(SwapFactory.GetAddressOf()));
	if (S_OK != SwapFactory->CreateSwapChain(Device.Get(), &SwapChainDesc, SwapChain.GetAddressOf()))
	{
		MsgAssert("����ü�� ������ �����߽��ϴ�.");
		return;
	}

	// ��� �� �ݵ�� Release
	SwapDevice.Reset();
	SwapAdapter.Reset();
	SwapFactory.Reset();

	// ������� �����͸� ������ ������ ���� ����ü���� ���������� �����ƴ��� Ȯ���� �� ����
	COMPTR<ID3D11Texture2D> SwapBackBufferTexture = nullptr;
	if (S_OK != SwapChain.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(SwapBackBufferTexture.GetAddressOf())))
	{
		MsgAssert("����ü�� ������ �����߽��ϴ�.");
		return;
	}
	// ����ü���� ���������� ���� ���� ���۸� �����ϴµ�, �� �� 0��° ����(ȭ�鿡 �׸� �����)�� ID3D11Texture2D�� ������ �� �ִ�.
	// ����ۿ� ���� �׸����� ����Ÿ�ٺ䰡 �ʿ��ϰ�, �� ����Ÿ�ٺ並 ������� ID3D11Texture2D�� �ʿ�

	// ����� ������(ID3D11Texture2D)�� ����Ÿ�ٺ� ����
	std::shared_ptr<Ext_DirectXTexture> BackBufferTexture = std::make_shared<Ext_DirectXTexture>();
	BackBufferTexture->CreateRenderTargetView(SwapBackBufferTexture);
	// Swap Chain ���� -> Swap Chain �ȿ��� �����(���� ü�� �ؽ�ó, 2D �ؽ�ó��)�� ������ -> ������ ����۸� ������ ����Ÿ�ٺ��� ������Ʈ�� �ٿ��
	// ����Ÿ�ٺ信 ���� ȭ���� �������Ѵٴ� ���� ����ۿ� �������ϴ� �Ͱ� ����.
	// �� ���ۿ� ��� ȭ���� �������ϰ� �� �������� SwapChain�� ���� ����۸� ȭ�鿡 �׸��� �Ǵ� ���̴�.

	// �������� ����� ����Ÿ�� ����
	MainRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget("MainRenderTarget", BackBufferTexture, { 0.0f, 0.0f, 1.0f, 1.0f });
	MainRenderTarget->CreateDepthTexture(); // ���̿� ���ٽ� ������ ���� �����ؽ��� ����
}

void Ext_DirectXDevice::RenderStart()
{
	MainRenderTarget->RenderTargetClear();
	MainRenderTarget->RenderTargetSetting();
}

void Ext_DirectXDevice::RenderEnd()
{
	HRESULT Result = SwapChain->Present(0, 0);
	if (Result == DXGI_ERROR_DEVICE_REMOVED || Result == DXGI_ERROR_DEVICE_RESET)
	{
		// ����̽� �ٽø����
		MsgAssert("����Ÿ�� ������ �����߽��ϴ�.");
		return;
	}
}