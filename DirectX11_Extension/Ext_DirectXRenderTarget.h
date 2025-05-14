#pragma once

#include "Ext_DirectXTexture.h"
#include "ResourceManager.h"

class Ext_DirectXRenderTarget : public ResourceManager<Ext_DirectXRenderTarget>
{
public:
	// constrcuter destructer
	Ext_DirectXRenderTarget() {};
	~Ext_DirectXRenderTarget() {};

	// delete Function
	Ext_DirectXRenderTarget(const Ext_DirectXRenderTarget& _Other) = delete;
	Ext_DirectXRenderTarget(Ext_DirectXRenderTarget&& _Other) noexcept = delete;
	Ext_DirectXRenderTarget& operator=(const Ext_DirectXRenderTarget& _Other) = delete;
	Ext_DirectXRenderTarget& operator=(Ext_DirectXRenderTarget&& _Other) noexcept = delete;

	static std::shared_ptr<Ext_DirectXRenderTarget> CreateRenderTarget(std::string_view _Name, std::shared_ptr<class Ext_DirectXTexture> _Texture, const float4& _Color)
	{
		std::shared_ptr<Ext_DirectXRenderTarget> NewRenderTarget = ResourceManager::CreateNameResource(_Name);
		NewRenderTarget->ResourceCreate(_Texture, _Color);
		return NewRenderTarget;
	}

	void CreateDepthTexture(int _Index = 0);

protected:
	
private:
	void ResourceCreate(std::shared_ptr<Ext_DirectXTexture> _Texture, float4 _Color);

	std::vector<float4> Color;

	std::vector<std::shared_ptr<Ext_DirectXTexture>> Textures = {};
	std::vector<D3D11_VIEWPORT> ViewPortDatas = {};
	std::vector<ID3D11RenderTargetView*> RTVs = {};
	std::vector<ID3D11ShaderResourceView*> SRVs = {};
	std::shared_ptr<Ext_DirectXTexture> DepthTexture = {};

};