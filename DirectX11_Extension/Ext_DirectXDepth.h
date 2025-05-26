#pragma once
#include "Ext_ResourceManager.h"

class Ext_DirectXDepth : public Ext_ResourceManager<Ext_DirectXDepth>
{
	friend class Ext_DirectXMaterial;

public:
	// constrcuter destructer
	Ext_DirectXDepth() {};
	~Ext_DirectXDepth() {};

	// delete Function
	Ext_DirectXDepth(const Ext_DirectXDepth& _Other) = delete;
	Ext_DirectXDepth(Ext_DirectXDepth&& _Other) noexcept = delete;
	Ext_DirectXDepth& operator=(const Ext_DirectXDepth& _Other) = delete;
	Ext_DirectXDepth& operator=(Ext_DirectXDepth&& _Other) noexcept = delete;

	static std::shared_ptr<Ext_DirectXDepth> CreateDepth(const std::string_view& _Name, const D3D11_DEPTH_STENCIL_DESC& _Desc)
	{
		std::shared_ptr<Ext_DirectXDepth> NewDepth = Ext_DirectXDepth::CreateNameResource(_Name);
		NewDepth->CreateDepth(_Desc);

		return NewDepth;
	}

protected:
	
private:
	void CreateDepth(const D3D11_DEPTH_STENCIL_DESC& _Value);
	void Setting();

	D3D11_DEPTH_STENCIL_DESC Desc = {};
	COMPTR<ID3D11DepthStencilState> DSS = nullptr;
	
};