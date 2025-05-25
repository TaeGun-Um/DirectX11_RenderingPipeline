#pragma once
#include "Ext_ResourceManager.h"

class Ext_DirectXRasterizer : public Ext_ResourceManager<Ext_DirectXRasterizer>
{
	friend class Ext_DirectXMaterial;

public:
	// constrcuter destructer
	Ext_DirectXRasterizer() {};
	~Ext_DirectXRasterizer() {};

	// delete Function
	Ext_DirectXRasterizer(const Ext_DirectXRasterizer& _Other) = delete;
	Ext_DirectXRasterizer(Ext_DirectXRasterizer&& _Other) noexcept = delete;
	Ext_DirectXRasterizer& operator=(const Ext_DirectXRasterizer& _Other) = delete;
	Ext_DirectXRasterizer& operator=(Ext_DirectXRasterizer&& _Other) noexcept = delete;

	static std::shared_ptr<Ext_DirectXRasterizer> CreateRasterizer(const std::string_view& _Name, const D3D11_RASTERIZER_DESC& _Desc)
	{
		std::shared_ptr<Ext_DirectXRasterizer> NewRasterizer = Ext_ResourceManager::CreateNameResource(_Name);
		NewRasterizer->CreateRasterizer(_Desc);
		
		return NewRasterizer;
	}

	inline void SetFILL_MODE(D3D11_FILL_MODE _Value)
	{
		switch (_Value)
		{
		case D3D11_FILL_WIREFRAME:
			CurState = WireframeState;
			break;
		case D3D11_FILL_SOLID:
			CurState = SolidState;
			break;
		default:
			break;
		}
	}

	inline void SetCULL_MODE(D3D11_CULL_MODE _Value)
	{
		Desc.CullMode = _Value;
		CreateRasterizer(Desc);
	}

protected:
	void CreateRasterizer(const D3D11_RASTERIZER_DESC& _Value);
	
private:
	void Setting();
	void Release();

	D3D11_RASTERIZER_DESC Desc = {};
	COMPTR<ID3D11RasterizerState> CurState = nullptr;
	COMPTR<ID3D11RasterizerState> SolidState = nullptr;
	COMPTR<ID3D11RasterizerState> WireframeState = nullptr;

};