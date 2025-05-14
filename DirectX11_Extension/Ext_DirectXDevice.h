#pragma once

// DirectX Device, Context 담당 클래스
class Ext_DirectXDevice
{
public:
	// constrcuter destructer
	Ext_DirectXDevice();
	~Ext_DirectXDevice();

	// delete Function
	Ext_DirectXDevice(const Ext_DirectXDevice& _Other) = delete;
	Ext_DirectXDevice(Ext_DirectXDevice&& _Other) noexcept = delete;
	Ext_DirectXDevice& operator=(const Ext_DirectXDevice& _Other) = delete;
	Ext_DirectXDevice& operator=(Ext_DirectXDevice&& _Other) noexcept = delete;

protected:
	
private:
	static ID3D11Device* Device;		 // 그래픽카드 RAM에 해당하는 부분에 대한 권한 담당
	static ID3D11DeviceContext* Context; // 랜더링에 관련된 연산 담당
	
};