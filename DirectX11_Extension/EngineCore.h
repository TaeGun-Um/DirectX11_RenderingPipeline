#pragma once

class EngineCore
{
public:
	// delete Function
	EngineCore(const EngineCore& _Other) = delete;
	EngineCore(EngineCore&& _Other) noexcept = delete;
	EngineCore& operator=(const EngineCore& _Other) = delete;
	EngineCore& operator=(EngineCore&& _Other) noexcept = delete;

	static void Run(HINSTANCE _hInstance);

protected:
	
private:
	// constrcuter destructer
	EngineCore() {};
	~EngineCore() {};
	
	static void Start();
	static void Update();
	static void End();
};