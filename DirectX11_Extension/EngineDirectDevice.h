#pragma once

// DirectX11의 Device, Context 담당 클래스
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