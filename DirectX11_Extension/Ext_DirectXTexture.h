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

	void CreateRenderTargetView(ID3D11Texture2D* _Texture);
	
	static std::shared_ptr<Ext_DirectXTexture> Create(const D3D11_TEXTURE2D_DESC& _Value)
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
	ID3D11RenderTargetView* GetRTV(size_t _Index = 0) { return RenderTargetViews[_Index]; }

protected:
	
private:
	void CreateView(const D3D11_TEXTURE2D_DESC& _Value);
	void CreateRenderTargetView();
	void CreateShaderResourcesView();
	void CreateDepthStencilView();

	ID3D11Texture2D* Texture2D = nullptr;
	ID3D11DepthStencilView* DepthStencilView = nullptr;
	ID3D11ShaderResourceView* ShaderResourceView = nullptr;
	std::vector<ID3D11RenderTargetView*> RenderTargetViews;
	std::vector<float4> Colors;

	D3D11_TEXTURE2D_DESC Desc;
};
// [ID3D11Texture2D]
//

// [ID3D11RenderTargetView]
// IDXGISwapChain���� SwapChain�� ���� ���� ���۸� ��ü�ϴµ�, �̶� Ư�� ���۸� �����ϵ��� Ÿ�����Ѵ�. �̿� ���� ���

// [ID3D11DepthStencilView]
// 

// [ID3D11ShaderResourceView]