#pragma once

// DirectX�� �ʿ��� Resource���� �ε��ϴ� Ŭ����
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