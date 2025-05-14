#pragma once

// DirectX Device, Context ��� Ŭ����
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
	static ID3D11Device* Device;		 // �׷���ī�� RAM�� �ش��ϴ� �κп� ���� ���� ���
	static ID3D11DeviceContext* Context; // �������� ���õ� ���� ���
	
};