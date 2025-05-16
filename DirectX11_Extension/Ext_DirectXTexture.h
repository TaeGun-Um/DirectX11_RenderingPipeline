#pragma once

#include "ResourceManager.h"

// DirectX�� Texture2D, View ���
class Ext_DirectXTexture : public ResourceManager<Ext_DirectXTexture>
{
public:
	// constrcuter destructer
	Ext_DirectXTexture() {};
	~Ext_DirectXTexture() {};

	// delete Function
	Ext_DirectXTexture(const Ext_DirectXTexture& _Other) = delete;
	Ext_DirectXTexture(Ext_DirectXTexture&& _Other) noexcept = delete;
	Ext_DirectXTexture& operator=(const Ext_DirectXTexture& _Other) = delete;
	Ext_DirectXTexture& operator=(Ext_DirectXTexture&& _Other) noexcept = delete;

	// ����� ����Ÿ�ٺ� ������ ���� ȣ���
	void CreateRenderTargetView(ID3D11Texture2D* _Texture);
	
	// ���ҽ� �Ŵ����� ���� ����ϰ�, Desc�� BindFlags�� ���� RTV, SRV, DSV �� �ϳ��� Create
	static std::shared_ptr<Ext_DirectXTexture> CreateViews(const D3D11_TEXTURE2D_DESC& _Value)
	{
		std::shared_ptr<Ext_DirectXTexture> NewTexture = ResourceManager::CreateResource();
		NewTexture->CreateView(_Value);
		return NewTexture;
	}

	// Getter, Setter
	int GetWidth() {	return Desc.Width; }
	int GetHeight() { return Desc.Height; }
	float4 GetScale() {	return float4(static_cast<float>(Desc.Width), static_cast<float>(Desc.Height)); }
	ID3D11ShaderResourceView* GetSRV() { return ShaderResourceView; }
	ID3D11RenderTargetView* GetRTV(size_t _Index = 0) { return RTVs[_Index]; }

protected:
	
private:
	void CreateView(const D3D11_TEXTURE2D_DESC& _Value); // Desc�� BindFlags�� ���� RTV, SRV, DSV �� �ϳ��� Create
	void CreateRenderTargetView(); // ����Ÿ�ٺ� ����
	void CreateDepthStencilView(); // �������ٽǺ� ����
	void CreateShaderResourcesView(); // ���̴����ҽ��� ����

	ID3D11Texture2D* Texture2D = nullptr;
	D3D11_TEXTURE2D_DESC Desc;

	std::vector<ID3D11RenderTargetView*> RTVs; // ������ ����Ÿ�ٺ���� �����ϴ� �����̳�
	ID3D11ShaderResourceView* ShaderResourceView = nullptr; // ������ ���̴� ���ҽ� ��
	ID3D11DepthStencilView* DepthStencilView = nullptr; // ������ �������ٽ� ��

	std::vector<float4> Colors;

};
// [ID3D11Texture2D]
// 2D �ؽ�ó�� ��Ÿ���� �������̽�, GPU�� �ö󰡴� 2���� �̹���(�ؽ�ó), ����Ÿ��, ���� ���� ���� �ڿ��� ǥ���ϴµ� ���

// [D3D11_TEXTURE2D_DESC]
// 2D �ؽ��ĸ� ������ �� ����ϴ� ����ü�� �ؽ��� �ػ�, ����, ���ø� ����, BindFlags(��� �뵵) ���� ��� ����
// <<����>>
/*1. Width : �ؽ�ó�� ���� �ػ�(�ȼ� ����)*/ 
/*2. Height : �ؽ�ó�� ���� �ػ�(�ȼ� ����)*/
/*3. MipLevels : ������ miplevel ��, 0�̸� Direct3D�� ��� �Ӹ� �ڵ� ����*/
/*4. ArraySize : �ؽ�ó �迭�� ����(�迭 �ؽ�ó �Ǵ� ť����� 6�� ��)*/
/*5. Format : �ȼ� ����(DXGI_FORMAT_R8G8B8A8_UNORM)*/
/*6. SampleDesc : ��Ƽ���ø��� ����(MSAA), Count, Quality ���� �������*/
/*7.Usage : �ؽ�ó ��� ����(DEFAULT, IMMUTABLE, DYNAMIC, STAGING)*/
/*8. BindFlags : ���ε� ������ ����(RENDER_TARGET, DEPTH_STENCIL, SHADER_RESOURCE)*/
/*9. CPUAccessFlags : CPU�� ������ ����(���� DYNAMIC�̳� STAGING���� ���)*/
/*10. MiscFlags : ��Ÿ �÷��� (TEXTURECUBE, GENERATE_MIPS ��)*/

// [ID3D11RenderTargetView] 
// GPU�� �ȼ� ����� ����� �� �ֵ��� ������ 2D �ؽ���(�׸���) ������ �ϴ� ��(View)�̴�.
// ����̽��� ����ۿ� �ٿ���Ǹ鼭 �����ȴ�.
// �� ���ۿ� ��� ȭ���� �������ϰ� �� �������� SwapChain�� ���� ����۸� ȭ�鿡 �׸��� �Ǵ� ���̴�.
// <<����>>
/*1. ���� Ÿ�� ���� : GPU ������������ ���� Ÿ�� ���Կ� ����Ǿ�, �ȼ� ���̴��� ����� ���⿡ ��ϵ�*/  
/*2. View : �ؽ�ó ���ҽ�(ID3D11Texture2D)�� �Ϻκ� �Ǵ� ��ü�� GPU�� ���� Ÿ������ ����� �� �ְ� ���ִ� ���� ����*/
/*3. Output : ���� ȭ�� ��¿� �����(���� ü�� �ؽ�ó) �Ǵ� �ٸ� �ؽ�ó�� ���� ����� ��ǥ�� ��*/

// [ID3D11ShaderResourceView]
// Shader�� GPU �ڿ�(���ҽ�)�� ������ �� �ֵ���(���� �� �ֵ���) ���ִ� ��(View)�̴�.
// �ַ� �ؽ�ó, ���� ���� ���ҽ��� ���̴����� ���� �����ϰ� ����� ������ �����Ѵ�.
// <<����>>
/*1. ���̴� ���ҽ� ���� : ���̴�(�ȼ� ���̴�, ���ؽ� ���̴� ��)���� �ؽ�ó�� ���� �����͸� �б� ���� �ʿ�*/
/*2. ���ҽ� View : ���� ���ҽ�(��: ID3D11Texture2D)�� Ư�� ������ ������ ���̴����� ���� �� �ֵ��� ����*/
/*3. ���ó : �ؽ�ó ���ø�, ���� ����, G-Buffer ����, ��ó��(PP) ȿ�� ��*/

// [ID3D11DepthStencilView]
// ����(Depth)�� ���ٽ�(Stencil) ������ �а� �� �� �ֵ��� ���ִ� ��(View)�̴�.
// ���� ����, ���� ����(Depth Buffer)�� ���ٽ� ����(Stencil Buffer)�� ����� �� �ִ� �ؽ�ó�� ���� ���� �������̽��̴�.
// ������ �������� GPU�� �ȼ��� �׸� ��, �� ���� �ֿ��� �׽�Ʈ(Depth Test, Stencil Test)�� ������ ���� �ִ�.
/*1. Depth Test (���� �׽�Ʈ) : �̹� �׷��� �ȼ����� ������� �Ǵ��� ���������� ����*/ 
/*2. Stencil Test (���ٽ� �׽�Ʈ) : ������ ������ �����ϴ� ��쿡�� �ȼ��� ������*/

