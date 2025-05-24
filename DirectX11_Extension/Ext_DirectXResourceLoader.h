#pragma once

// DirectX�� �ʿ��� Resource���� �ε��ϴ� Ŭ����
class Ext_DirectXResourceLoader
{
	friend class Ext_Core;

public:
	// delete Function
	Ext_DirectXResourceLoader(const Ext_DirectXResourceLoader& _Other) = delete;
	Ext_DirectXResourceLoader(Ext_DirectXResourceLoader&& _Other) noexcept = delete;
	Ext_DirectXResourceLoader& operator=(const Ext_DirectXResourceLoader& _Other) = delete;
	Ext_DirectXResourceLoader& operator=(Ext_DirectXResourceLoader&& _Other) noexcept = delete;

	static COMPTR<ID3D11RasterizerState> RasterState; // �ӽ�

protected:
	
private:
	static void Initialize();					// ���� 1ȸ �����Ͽ� ���ҽ� �ε�
	static void MakeVertex();			// ��ǲ���̾ƿ�, ���ؽ� ����, �ε��� ����, �޽� ����
	static void MakeSampler();			// ���÷� ����
	static void MakeBlend();				// ���� ����
	static void MakeDepth();				// ���� ����
	static void ShaderCompile();		// ���̴� ���� ������ �ǽ�
	static void MakeRasterizer();		// �����Ͷ����� ����
	static void MakeMaterial();			// ��Ƽ���� ����

};