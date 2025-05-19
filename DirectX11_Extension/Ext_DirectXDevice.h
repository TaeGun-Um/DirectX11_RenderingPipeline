#pragma once

// DirectX Device, Context 담당 클래스
class Ext_DirectXDevice
{
public:
	// constrcuter destructer
	Ext_DirectXDevice() {};
	~Ext_DirectXDevice() {};

	// delete Function
	Ext_DirectXDevice(const Ext_DirectXDevice& _Other) = delete;
	Ext_DirectXDevice(Ext_DirectXDevice&& _Other) noexcept = delete;
	Ext_DirectXDevice& operator=(const Ext_DirectXDevice& _Other) = delete;
	Ext_DirectXDevice& operator=(Ext_DirectXDevice&& _Other) noexcept = delete;

	static void Initialize(); // DirectX11 시작

	// Getter, Setter
	static COMPTR<ID3D11Device>& GetDevice() { return Device; };
	static COMPTR<ID3D11DeviceContext>& GetContext() { return Context; };
	static COMPTR<IDXGISwapChain>& GetSwapChain() { return SwapChain; };
	static std::shared_ptr<class Ext_DirectXRenderTarget> GetMainRenderTarget() { return MainRenderTarget; };

protected:
	
private:
	static COMPTR<IDXGIAdapter> GetHighPerformanceAdapter(); // 그래픽카드 정보 가져오기
	static void CreateSwapChain(); // 스왑체인 생성하기

	static COMPTR<ID3D11Device> Device;				// GPU를 제어할 수 있는 인터페이스
	static COMPTR<ID3D11DeviceContext> Context;   // GPU를 제어할 수 있는 인터페이스이지만 추가 기능이 있음(렌더링 관련 연산)
	static COMPTR<IDXGISwapChain> SwapChain;     // 화면을 표시하는 스왑체인 기능을 다룸
	static std::shared_ptr<class Ext_DirectXRenderTarget> MainRenderTarget; // BackBuffer RenderTarget
	
};
// [ID3D11Device]
// GPU를 제어할 수 있는 인터페이스, 이 인터페이스는 기능 지원 점검과 자원 할당에 사용됨(그래픽카드 RAM에 해당하는 부분에 대한 권한 담당)

// [ID3D11DeviceContext]
// 이것도 GPU를 제어할 수 있는 인터페이스이지만 추가 기능이 있음
// 1. 렌더 대상 설정
// 2. ID3D11Device로 할당된 자원을 그래픽 파이프 라인에 묶고, GPU가 렌더링 명령을 지시하도록 함

// [DXGI]
// DirectX11 등 여러 그래픽스 기능이나 어플리케이션으로부터 오는 표시를 받아 커널 모드 드라이버나 하드웨어와 주고받는 역할 수행
// 그래픽카드 선택, 윈도우 사이즈 갱신(백버퍼 사이즈 갱신, 디스플레이 모드 갱신), 필요없는 화면 렌더링 제어, 화면 모드 전환(풀 <-> 윈도우), 톤 커브에 의한 계조 보정 등 수행
// 1. IDXGIFactory : DXGi를 사용하기 위해 필요한 각종 인터페이스를 얻어오는 인터페이스, CreateDXGIFactory()로 인터페이스를 얻어온다.
// 2. IDXGIAdapter : 그래픽카드 관련 기능을 다룸
// 3. IDXGIOutput : 그래픽카드에 연결된 디스플레이  관련 기능을 다룸, 이걸로 대응 디스플레이 모드(해상도, Refresh), 감마 설정 등 가능
// 4. IDXGISwapChain : 화면을 표시하는 스왑체인 기능을 다룸, D3D11CreateDeviceAndSwapChain로 ID3D11Device 인터페이스를 얻을 때 같이 얻을 수 있음

// [SwapChain]
// 엔진에서 GPU가 프레임 버퍼에 이미지를 그리는 동안, 비디오 컨트롤러가 GPU가 이미지를 그리고 있는 프레임 버퍼를 참조하여 화면에 출력할 때 깜빡거리는 현상이 있는데,
// 이를 해결하기 위해 GPU가 그리는 백 프레임 버퍼와 화면에 출력하는 프론트 프레임 버퍼 두 개를 만들어서 교체하는 방식을 말함
// 프레임 버퍼간 계속된 교체가 일어남, 백이 프론트가 되면 프론트가 백이 되고, 이걸 반복
// 이를 이중 버퍼링이라고도 부르며, 버퍼 개수를 늘려도 되지만 보통 2개만 해도 깜빡거림이 해결됨
