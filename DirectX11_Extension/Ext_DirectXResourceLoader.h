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
	static COMPTR<ID3D11InputLayout> InputLayOut;

protected:
	
private:
	static void MakeVertex();
	static void MakeSampler();
	static void MakeBlend();
	static void MakeDepth();
	static void ShaderCompile();
	static void MakeRasterizer();
	static void MakeMaterial();

};