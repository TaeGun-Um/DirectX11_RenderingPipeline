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

	// ID3D11RasterizerState 생성
	static std::shared_ptr<Ext_DirectXRasterizer> CreateRasterizer(std::string_view _Name, const D3D11_RASTERIZER_DESC& _Desc)
	{
		std::shared_ptr<Ext_DirectXRasterizer> NewRasterizer = Ext_ResourceManager::CreateNameResource(_Name);
		NewRasterizer->CreateRasterizer(_Desc);
		
		return NewRasterizer;
	}

	// FILL_MODE 선택
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
		RaterizerInfo.CullMode = _Value;
		CreateRasterizer(RaterizerInfo);
	}

protected:
	void CreateRasterizer(const D3D11_RASTERIZER_DESC& _Value);
	
private:
	void Setting();
	void Release();

	D3D11_RASTERIZER_DESC RaterizerInfo = {}; // 레스터라이저 설정 값
	COMPTR<ID3D11RasterizerState> CurState = nullptr; // 현재 지정된 레스터라이저 상태, 기본은 Solid
	COMPTR<ID3D11RasterizerState> SolidState = nullptr; // Solid로 레스터라이저 활용
	COMPTR<ID3D11RasterizerState> WireframeState = nullptr; // Wireframe으로 레스터라이저 활용

};