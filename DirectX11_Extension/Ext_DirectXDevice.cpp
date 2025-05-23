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

// DirectX11 시작
void Ext_DirectXDevice::Initialize()
{
	if (!Base_Windows::GetHWnd())
	{
		MsgAssert("윈도우 초기화 전에는 Initialize를 진행할 수 없습니다.");
		return;
	}
	
	int iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG; // DirectX 11 디버그 기능 설정

#endif
	D3D_FEATURE_LEVEL Level = D3D_FEATURE_LEVEL_11_0;
	COMPTR<IDXGIAdapter> Adapter = GetHighPerformanceAdapter();
	if (!Adapter)
	{
		MsgAssert("그래픽카드 어댑터 인터페이스 획득 실패");
		return;
	}

	// 디바이스, 디바이스 컨텍스트 생성 함수
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
	// <<설명>>
	// [1] IDXGIAdapter*						: 디바이스를 만들 때 사용할 비디오 어댑터에 대한 포인터, nullptr 전달 시 IDXGIFactory1::EnumAdapters로 열거된 첫 번째 어댑터인 기본값이 설정됨
	// [2] D3D_DRIVER_TYPE				: 드라이버 유형 선택, 어댑터를 전달했으니 UNKNOWN을 전달하면 됨(HARDWARE == GPU 사용, REFERENCE == CPU 기반 소프트웨어 구현, WARP == 고성능 소프트웨어 구현)
	// [3] HMODULE								: 소프트웨어 래스터라이저를 구현하는 DLL에 대한 핸들, nullptr 전달 시 소프트웨어 드라이버 없음을 전달, 보통 nullptr을 전달한다.
	// [4] UINT									: D3D11_CREATE_DEVICE_FLAG 값 전달, 디버그 레이어 플래그
	// [5] const D3D_FEATURE_LEVEL   : 만드는 기능 수준의 순서를 결정하는 Feature Level 배열에 대한 포인터 전달, nullptr 전달 시 D3D_FEATURE_LEVEL 배열의 첫 번째 값이 반환됨
	// [6] UINT									: [5]에 대한 배열 요소의 개수
	// [7] UINT									: SDK 버전, 무조건 D3D11_SDK_VERSION로 전달
	// [8] ID3D11Device**					: 만든 디바이스를 반환해주니 받으면 됨
	// [9] D3D_FEATURE_LEVEL*			: 실제로 생성된 디바이스의 기능 수준 반환, 첫 번째 D3D_FEATURE_LEVEL 반환해줌
	// [10] ID3D11DeviceContext**		: 디바이스컨텍스트 반환해주니 받으면 됨
	
	// Feature Level은 DirectX에서 GPU가 지원하는 기능을 단계별로 구분한 것이다. -> 이 그래픽카드가 얼마나 최신 기술을 쓸 수 있는가?
	// DirectX11을 사용하고 싶어도 GPU가 기능 지원 안하면 안되기때문에, 이걸로 받아서 확인해보면 된다.

	if (S_OK != Hr)
	{
		MsgAssert("디바이스 생성에 실패했습니다.");
		return;
	}

	// 어댑터 다썼으면 해제
	if (nullptr != Adapter)
	{
		Adapter.Reset();
	}

	// DirectX11로 초기화된게 아니라면 해당 어댑터는 DirectX11을 지원하지 않는 그래픽카드임
	if (Level != D3D_FEATURE_LEVEL_11_0)
	{
		MsgAssert("다이렉트 11을 지원하지 않는 그래픽카드 입니다");
		return;
	}

	Hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED); // 멀티스레드 사용 설정
	CreateSwapChain(); // 스왑체인 생성
}

// 그래픽카드 정보 가져오기
COMPTR<IDXGIAdapter> Ext_DirectXDevice::GetHighPerformanceAdapter()
{
	COMPTR<IDXGIFactory> Factory = nullptr;
	COMPTR<IDXGIAdapter> Adapter = nullptr;

	// 팩토리 생성
	HRESULT HR = CreateDXGIFactory
	(
		__uuidof(IDXGIFactory),				// [1] 생성할 인터페이스의 GUID를 지정, IDXGIFactory는 DXGI 팩토리 인터페이스이며 __uuidof는 컴파일 타임에 GUID를 추출하는 매크로
		(void**)Factory.GetAddressOf()	// [2] 결과로 생성된 IDXGIFactory 객체를 받을 포인터를 전달
	);

	if (nullptr == Factory)
	{
		MsgAssert("그래픽카드에서 팩토리 인터페이스를 생성하지 못했습니다.");
		return COMPTR<IDXGIAdapter>();
	}

	// 여러 GPU 중에서, "가장 많은 전용 비디오 메모리(DedicatedVideoMemory)"를 가진 어댑터를 선택
	size_t prevAdapterVideoMemory = 0;
	for (UINT Adapterindex = 0; ; Adapterindex++)
	{
		COMPTR<IDXGIAdapter> CurAdapter = nullptr;
		Factory->EnumAdapters(Adapterindex, CurAdapter.GetAddressOf()); // DXGI 팩토리를 통해 어댑터를 하나 얻음

		if (nullptr == CurAdapter) // 어댑터가 더 이상 없으면 루프 종료
		{
			break;
		}

		DXGI_ADAPTER_DESC Desc;
		CurAdapter->GetDesc(&Desc); // 현재 어댑터의 정보를 DXGI_ADAPTER_DESC에 채움

		if (prevAdapterVideoMemory <= Desc.DedicatedVideoMemory) // 이전에 선택된 어댑터보다 메모리가 같거나 많다면 갱신
		{
			prevAdapterVideoMemory = Desc.DedicatedVideoMemory;

			if (nullptr != Adapter)
			{
				Adapter.Reset(); // 이전 어댑터 메모리 해제
			}

			Adapter = CurAdapter; // 현재 어댑터를 최종 후보로 저장
			continue;
		}

		CurAdapter.Reset(); // 사용 후 반드시 Release
	}

	Factory.Reset(); // 사용 후 반드시 Release 해주도록 한다.

	return Adapter;
}

// 스왑체인 생성
void Ext_DirectXDevice::CreateSwapChain()
{
	float4 ScreenSize = Base_Windows::GetScreenSize();
	DXGI_SWAP_CHAIN_DESC SwapChainDesc = { 0, }; // DXGI_SWAP_CHAIN_DESC 구조체 생성

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
	// <<설명>>
	// [1] : 더블 버퍼링 사용, GPU가 하나의 버퍼에 렌더링하는 동안 다른 하나는 화면에 표시하여 깜빡임과 티어링 방지
	// [2] : 스왑체인 버퍼 해상도 설정(너비)
	// [3] : 스왑체인 버퍼 해상도 설정(높이)
	// [4] : 렌더링 결과를 출력할 윈도우의 핸들 설정, CreateWindow()로 만든 창의 핸들(현재 실행하는 프로세스 핸들 넣어줌)
	// [5] : 화면 주사율 설정, 여기서 1
	// [6] : 화면 주사율 설정, 여기서 60, 이러면 Numerator/ Denominator가 되어 60 / 1 = 60Hz로 설정됨
	// [7] : 픽셀 포멧 설정, DXGI_FORMAT_R8G8B8A8_UNORM는 각 RGBA 채널이 8bit이며 0~255 범위의 정규화 된 값을 가짐
	// [8] : 스캔라인 순서를 명시하지 않는다는 뜻으로 기본 설정하라고 전달
	// [9] : 화면 확대/축소 모드 설정, DXGI_MODE_SCALING_UNSPECIFIED는 디스플레이 기본 설정을 따른다는 뜻이다.
	// [10] : 해당 스왑 체인을 렌더 타겟으로 사용하고, 쉐이더도 해당 옵션을 사용하도록 설정
	// [11] : Count가 1이면 안티앨리어싱 사용 안한다는 뜻
	// [12] : 멀티 샘플링(MSAA) 비활성화
	// [13] : 최신 방식의 플립 체인 사용한다는 뜻
	// [14] : 전체화면 전환 시 해상도 변경을 허용할 것인지, 해당 프로젝트는 안쓰도록 설정
	// [15] : 창모드 설정(false는 전체화면 모드)

	COMPTR<IDXGIDevice> SwapDevice = nullptr;
	COMPTR<IDXGIAdapter> SwapAdapter = nullptr;
	COMPTR<IDXGIFactory> SwapFactory = nullptr;

	// 디바이스 쿼리인터페이스에서 SwapDevice 값 복사
	Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(SwapDevice.GetAddressOf()));
	if (nullptr == SwapDevice)
	{
		MsgAssert("DXGI 디바이스를 DirectX디바이스에서 얻어오지 못했습니다.");
		return;
	}

	// SwapDevice에서 SwapAdapter 값 복사
	SwapDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(SwapAdapter.GetAddressOf()));
	if (nullptr == SwapAdapter)
	{
		MsgAssert("DXGI 디바이스를 DirectX디바이스에서 얻어오지 못했습니다.");
		return;
	}

	// SwapAdapter에서 SwapFactory 값을 복사한 뒤, 그로부터 스왑체인을 생성함
	SwapAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(SwapFactory.GetAddressOf()));
	if (S_OK != SwapFactory->CreateSwapChain(Device.Get(), &SwapChainDesc, SwapChain.GetAddressOf()))
	{
		MsgAssert("스왑체인 생성에 실패했습니다.");
		return;
	}

	// 사용 후 반드시 Release
	SwapDevice.Reset();
	SwapAdapter.Reset();
	SwapFactory.Reset();

	// 백버퍼의 포인터를 얻어오는 과정을 통해 스왑체인이 정상적으로 생성됐는지 확인할 수 있음
	COMPTR<ID3D11Texture2D> SwapBackBufferTexture = nullptr;
	if (S_OK != SwapChain.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(SwapBackBufferTexture.GetAddressOf())))
	{
		MsgAssert("스왑체인 생성에 실패했습니다.");
		return;
	}
	// 스왑체인은 내부적으로 여러 개의 버퍼를 관리하는데, 그 중 0번째 버퍼(화면에 그릴 백버퍼)를 ID3D11Texture2D로 가져올 수 있다.
	// 백버퍼에 직접 그리려면 렌더타겟뷰가 필요하고, 이 렌더타겟뷰를 만드려면 ID3D11Texture2D가 필요

	// 백버퍼 포인터(ID3D11Texture2D)로 랜더타겟뷰 생성
	std::shared_ptr<Ext_DirectXTexture> BackBufferTexture = std::make_shared<Ext_DirectXTexture>();
	BackBufferTexture->CreateRenderTargetView(SwapBackBufferTexture);
	// Swap Chain 생성 -> Swap Chain 안에서 백버퍼(스왑 체인 텍스처, 2D 텍스처임)를 꺼내옴 -> 꺼내온 백버퍼를 가지고 랜더타겟뷰라는 오브젝트에 바운딩
	// 렌더타겟뷰에 게임 화면을 렌더링한다는 것은 백버퍼에 렌더링하는 것과 같다.
	// 백 버퍼에 모든 화면을 렌더링하고 난 다음에는 SwapChain을 통해 백버퍼를 화면에 그리면 되는 것이다.

	// 메인으로 사용할 랜더타겟 생성
	MainRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget("MainRenderTarget", BackBufferTexture, { 0.0f, 0.0f, 1.0f, 1.0f });
	MainRenderTarget->CreateDepthTexture(); // 깊이와 스텐실 정보를 위한 뎁스텍스쳐 생성
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
		// 디바이스 다시만들기
		MsgAssert("랜더타겟 생성에 실패했습니다.");
		return;
	}
}