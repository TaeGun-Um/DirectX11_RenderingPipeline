#pragma once

// DirectX11�� Device, Context ��� Ŭ����
class EngineDirectDevice
{
public:
	// constrcuter destructer
	EngineDirectDevice();
	~EngineDirectDevice();

	// delete Function
	EngineDirectDevice(const EngineDirectDevice& _Other) = delete;
	EngineDirectDevice(EngineDirectDevice&& _Other) noexcept = delete;
	EngineDirectDevice& operator=(const EngineDirectDevice& _Other) = delete;
	EngineDirectDevice& operator=(EngineDirectDevice&& _Other) noexcept = delete;

protected:
	
private:

	
};