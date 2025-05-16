#pragma once

// DirectX에 필요한 Resource들을 로드하는 클래스
class Ext_DirectXResourceLoader
{
public:
	// delete Function
	Ext_DirectXResourceLoader(const Ext_DirectXResourceLoader& _Other) = delete;
	Ext_DirectXResourceLoader(Ext_DirectXResourceLoader&& _Other) noexcept = delete;
	Ext_DirectXResourceLoader& operator=(const Ext_DirectXResourceLoader& _Other) = delete;
	Ext_DirectXResourceLoader& operator=(Ext_DirectXResourceLoader&& _Other) noexcept = delete;

	static void Initialize();

protected:
	
private:
	// constrcuter destructer
	Ext_DirectXResourceLoader() {};
	~Ext_DirectXResourceLoader() {};
	
};