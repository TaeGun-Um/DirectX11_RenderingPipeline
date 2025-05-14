#pragma once

// Process¿« Loop, shutdown process ¥„¥Á
class Ext_Core
{
public:
	// delete Function
	Ext_Core(const Ext_Core& _Other) = delete;
	Ext_Core(Ext_Core&& _Other) noexcept = delete;
	Ext_Core& operator=(const Ext_Core& _Other) = delete;
	Ext_Core& operator=(Ext_Core&& _Other) noexcept = delete;

	static void Run(HINSTANCE _hInstance, const float4& _ScreenSize, bool _IsFullScreen);

protected:
	
private:
	// constrcuter destructer
	Ext_Core() {};
	~Ext_Core() {};
	
	static void Start();
	static void Update();
	static void End();
};