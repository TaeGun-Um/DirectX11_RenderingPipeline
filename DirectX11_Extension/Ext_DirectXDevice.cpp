#include "PrecompileHeader.h"
#include "Ext_DirectXDevice.h"
#include <DirectX11_Base/Base_Debug.h>
#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Math.h>

ID3D11Device* Ext_DirectXDevice::Device = nullptr;
ID3D11DeviceContext* Ext_DirectXDevice::Context = nullptr;
IDXGISwapChain* Ext_DirectXDevice::SwapChain = nullptr;

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
	IDXGIAdapter* Adapter = GetHighPerformanceAdapter();
	if (!Adapter)
	{
		MsgAssert("�׷���ī�� ����� �������̽� ȹ�� ����");
		return;
	}

	HRESULT Hr = D3D11CreateDevice
	(
		Adapter,                       // �⺻ �����
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_UNKNOWN,  // �ϵ���� ����
		nullptr,                         // ����Ʈ���� ����̹� ����
		iFlag,                            // ����� ���̾� �÷���
		nullptr,                         // ����� Feature Level
		0,                                // Feature Level ��
		D3D11_SDK_VERSION, // SDK ����
		&Device,                      // ����̽� ��ȯ
		&Level,						  // ���õ� Feature Level ��ȯ
		&Context                     // ����̽� ���ؽ�Ʈ ��ȯ
	);

	if (S_OK != Hr)
	{
		MsgAssert("����̽� ������ �����߽��ϴ�.");
		return;
	}

	// ����� �ٽ����� ����
	if (nullptr != Adapter)
	{
		Adapter->Release();
		Adapter = nullptr;
	}

	// DirectX11�� �ʱ�ȭ�Ȱ� �ƴ϶�� 
	if (Level != D3D_FEATURE_LEVEL_11_0)
	{
		MsgAssert("���̷�Ʈ 11�� �������� �ʴ� �׷���ī�� �Դϴ�");
		return;
	}

	// ��Ƽ������ ��� ����
	Hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// ����ü�� ����
	CreateSwapChain();

	int a = 0;
}

// �׷���ī�� ���� ��������
IDXGIAdapter* Ext_DirectXDevice::GetHighPerformanceAdapter()
{
	IDXGIFactory* Factory = nullptr;
	IDXGIAdapter* Adapter = nullptr;

	// ���丮 ����, c++���� �����ϴ� Ŭ������ �����ϱ� ���� GUI�� ������ �����̴�.
	// ����̽������� ���ο� ������ �ִ� �����ͳ� �ɹ������� ��������
	// __uuidof(IDXGIAdapter) ���� GUID�� �־���� �Ѵ�.
	// �̰��� ���α׷��� ��Ʋ� �� 1���� �����ϴ� "Key"�� ����� ����̴�.
	// MIDL_INTERFACE("2411e7e1-12ac-4ccf-bd14-9798e8534dc0") << �̷� ���� ���� GUID ���̶�� ���� �ȴ�.
	HRESULT HR = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&Factory);

	if (nullptr == Factory)
	{
		MsgAssert("�׷���ī�忡�� ���丮 �������̽��� �������� ���߽��ϴ�.");
		return nullptr;
	}

	size_t prevAdapterVideoMemory = 0;

	for (UINT Adapterindex = 0; ; Adapterindex++)
	{
		IDXGIAdapter* CurAdapter = nullptr;
		Factory->EnumAdapters(Adapterindex, &CurAdapter); // ���丮�� ���� ��͸� ���´�.

		if (nullptr == CurAdapter)
		{
			break;
		}

		DXGI_ADAPTER_DESC Desc;
		CurAdapter->GetDesc(&Desc); // ���丮�� ���� ��͸� ���� DXGI_ADAPTER_DESC�� �� ����

		if (prevAdapterVideoMemory <= Desc.DedicatedVideoMemory)
		{
			prevAdapterVideoMemory = Desc.DedicatedVideoMemory;

			if (nullptr != Adapter)
			{
				Adapter->Release(); // ��� �� �ݵ�� Release ���ֵ��� �Ѵ�.
			}

			Adapter = CurAdapter;
			continue;
		}

		CurAdapter->Release(); // ��� �� �ݵ�� Release ���ֵ��� �Ѵ�.
	}

	Factory->Release(); // ��� �� �ݵ�� Release ���ֵ��� �Ѵ�.

	return Adapter;
}

// ����ü�� ����
void Ext_DirectXDevice::CreateSwapChain()
{
	float4 ScreenSize = Base_Windows::GetScreenSize();

	DXGI_SWAP_CHAIN_DESC SwapChainDesc = { 0, };

	SwapChainDesc.BufferCount = 2;                                                         // �⺻����
	SwapChainDesc.BufferDesc.Width = ScreenSize.uix();
	SwapChainDesc.BufferDesc.Height = ScreenSize.uiy();
	SwapChainDesc.OutputWindow = Base_Windows::GetHWnd();
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;                                  // ȭ�� ���ŷ�
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                          // �׷����̹��� ����
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                      // ���ȳ�
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT; // DXGI_USAGE_RENDER_TARGET_OUTPUT == �����쿡 ���𰡸� ���� �뵵 // DXGI_USAGE_SHADER_INPUT == ���̴��� �ش� �ɼ� ����Ѵ�.
	SwapChainDesc.SampleDesc.Quality = 0;                                                  // ��Ƽ����̾���� �ڵ����� ����
	SwapChainDesc.SampleDesc.Count = 1;                                                    // ���ȳ�
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;                          // ū �ǹ̴� ����, ȭ�� ������ ���� ���ɼ��� ���ϴ� ���ε�, �츮�� �Ⱦ� ����
	SwapChainDesc.Windowed = true;                                                         // false�� ��üȭ��

	// ���丮 ����, c++���� �����ϴ� Ŭ������ �����ϱ� ���� GUI�� ������ �����̴�.
	// ����̽������� ���ο� ������ �ִ� �����ͳ� �ɹ������� ��������
	// __uuidof(IDXGIAdapter) ���� GUID�� �־���� �Ѵ�.
	// �̰��� ���α׷��� ��Ʋ� �� 1���� �����ϴ� "Key"�� ����� ����̴�.
	// MIDL_INTERFACE("2411e7e1-12ac-4ccf-bd14-9798e8534dc0") << �̷� ���� ���� GUID ���̶�� ���� �ȴ�.

	IDXGIDevice* SwapDevice = nullptr;
	IDXGIAdapter* SwapAdapter = nullptr;
	IDXGIFactory* SwapFactory = nullptr;

	// ����̽� �����������̽����� SwapDevice �� ����
	Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&SwapDevice));
	if (nullptr == SwapDevice)
	{
		MsgAssert("DXGI ����̽��� DirectX����̽����� ������ ���߽��ϴ�.");
		return;
	}

	// SwapDevice���� SwapAdapter �� ����
	SwapDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&SwapAdapter));
	if (nullptr == SwapAdapter)
	{
		MsgAssert("DXGI ����̽��� DirectX����̽����� ������ ���߽��ϴ�.");
		return;
	}

	// SwapAdapter���� SwapFactory ���� ������ ��, �׷κ��� ����ü���� ������
	SwapAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&SwapFactory));
	if (S_OK != SwapFactory->CreateSwapChain(Device, &SwapChainDesc, &SwapChain))
	{
		MsgAssert("����ü�� ������ �����߽��ϴ�.");
		return;
	}

	// ����� ���� Release�� �ǽ��Ͽ� �� ����
	SwapDevice->Release();
	SwapAdapter->Release();
	SwapFactory->Release();

	// ����Ÿ���� DC�� ��� ���� �ȴ�.
	ID3D11Texture2D* SwapBackBufferTexture = nullptr;

	// ����ü���� ���������� �����Ǿ��ٸ�, HRESULT�� S_OK�� ���޵ȴ�.
	HRESULT Result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&SwapBackBufferTexture));
	if (S_OK != Result)
	{
		MsgAssert("����ü�� ������ �����߽��ϴ�.");
		return;
	}

	// ����ü��(�����) ���� ��, �ؽ��Ŀ��� ����Ÿ�ٺ�(RTV) ����
	//std::shared_ptr<GameEngineTexture> BackBufferTexture = std::make_shared<GameEngineTexture>();
	//BackBufferTexture->ResCreate(SwapBackBufferTexture);

	//// ����Ÿ�ٺ� ���� ��, ���η���Ÿ�� ����
	//BackBufferTarget = GameEngineRenderTarget::Create("MainBackBufferTarget", BackBufferTexture, { 0.0f, 0.0f, 1.0f, 1.0f });
	//BackBufferTarget->CreateDepthTexture();
}