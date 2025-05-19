#pragma once

// DirectX Device, Context ��� Ŭ����
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

	static void Initialize(); // DirectX11 ����

	// Getter, Setter
	static COMPTR<ID3D11Device>& GetDevice() { return Device; };
	static COMPTR<ID3D11DeviceContext>& GetContext() { return Context; };
	static COMPTR<IDXGISwapChain>& GetSwapChain() { return SwapChain; };
	static std::shared_ptr<class Ext_DirectXRenderTarget> GetMainRenderTarget() { return MainRenderTarget; };

protected:
	
private:
	static COMPTR<IDXGIAdapter> GetHighPerformanceAdapter(); // �׷���ī�� ���� ��������
	static void CreateSwapChain(); // ����ü�� �����ϱ�

	static COMPTR<ID3D11Device> Device;				// GPU�� ������ �� �ִ� �������̽�
	static COMPTR<ID3D11DeviceContext> Context;   // GPU�� ������ �� �ִ� �������̽������� �߰� ����� ����(������ ���� ����)
	static COMPTR<IDXGISwapChain> SwapChain;     // ȭ���� ǥ���ϴ� ����ü�� ����� �ٷ�
	static std::shared_ptr<class Ext_DirectXRenderTarget> MainRenderTarget; // BackBuffer RenderTarget
	
};
// [ID3D11Device]
// GPU�� ������ �� �ִ� �������̽�, �� �������̽��� ��� ���� ���˰� �ڿ� �Ҵ翡 ����(�׷���ī�� RAM�� �ش��ϴ� �κп� ���� ���� ���)

// [ID3D11DeviceContext]
// �̰͵� GPU�� ������ �� �ִ� �������̽������� �߰� ����� ����
// 1. ���� ��� ����
// 2. ID3D11Device�� �Ҵ�� �ڿ��� �׷��� ������ ���ο� ����, GPU�� ������ ����� �����ϵ��� ��

// [DXGI]
// DirectX11 �� ���� �׷��Ƚ� ����̳� ���ø����̼����κ��� ���� ǥ�ø� �޾� Ŀ�� ��� ����̹��� �ϵ����� �ְ�޴� ���� ����
// �׷���ī�� ����, ������ ������ ����(����� ������ ����, ���÷��� ��� ����), �ʿ���� ȭ�� ������ ����, ȭ�� ��� ��ȯ(Ǯ <-> ������), �� Ŀ�꿡 ���� ���� ���� �� ����
// 1. IDXGIFactory : DXGi�� ����ϱ� ���� �ʿ��� ���� �������̽��� ������ �������̽�, CreateDXGIFactory()�� �������̽��� ���´�.
// 2. IDXGIAdapter : �׷���ī�� ���� ����� �ٷ�
// 3. IDXGIOutput : �׷���ī�忡 ����� ���÷���  ���� ����� �ٷ�, �̰ɷ� ���� ���÷��� ���(�ػ�, Refresh), ���� ���� �� ����
// 4. IDXGISwapChain : ȭ���� ǥ���ϴ� ����ü�� ����� �ٷ�, D3D11CreateDeviceAndSwapChain�� ID3D11Device �������̽��� ���� �� ���� ���� �� ����

// [SwapChain]
// �������� GPU�� ������ ���ۿ� �̹����� �׸��� ����, ���� ��Ʈ�ѷ��� GPU�� �̹����� �׸��� �ִ� ������ ���۸� �����Ͽ� ȭ�鿡 ����� �� �����Ÿ��� ������ �ִµ�,
// �̸� �ذ��ϱ� ���� GPU�� �׸��� �� ������ ���ۿ� ȭ�鿡 ����ϴ� ����Ʈ ������ ���� �� ���� ���� ��ü�ϴ� ����� ����
// ������ ���۰� ��ӵ� ��ü�� �Ͼ, ���� ����Ʈ�� �Ǹ� ����Ʈ�� ���� �ǰ�, �̰� �ݺ�
// �̸� ���� ���۸��̶�� �θ���, ���� ������ �÷��� ������ ���� 2���� �ص� �����Ÿ��� �ذ��
