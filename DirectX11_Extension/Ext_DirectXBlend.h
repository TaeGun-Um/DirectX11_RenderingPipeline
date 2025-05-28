#pragma once
#include "Ext_ResourceManager.h"

// DirectX11의 블렌드 속성을 관리하는 클래스
class Ext_DirectXBlend : public Ext_ResourceManager<Ext_DirectXBlend>
{
public:
	// constrcuter destructer
	Ext_DirectXBlend() {};
	~Ext_DirectXBlend() {};

	// delete Function
	Ext_DirectXBlend(const Ext_DirectXBlend& _Other) = delete;
	Ext_DirectXBlend(Ext_DirectXBlend&& _Other) noexcept = delete;
	Ext_DirectXBlend& operator=(const Ext_DirectXBlend& _Other) = delete;
	Ext_DirectXBlend& operator=(Ext_DirectXBlend&& _Other) noexcept = delete;

	// 블렌드 생성
	static std::shared_ptr<Ext_DirectXBlend> CreateBlend(std::string_view _Name, const D3D11_BLEND_DESC& _Desc)
	{
		std::shared_ptr<Ext_DirectXBlend> Res = Ext_ResourceManager::CreateNameResource(_Name);
		Res->CreateBlend(_Desc);

		return Res;
	}

	void Setting();

protected:
	
private:
	void CreateBlend(const D3D11_BLEND_DESC& _Value);

	D3D11_BLEND_DESC BlendInfo = {}; // 블렌드 설명서
	COMPTR<ID3D11BlendState> BlendState = nullptr; // 생성된 블렌드 저장
	
	UINT Mask = 0xFFFFFFFF;
	
};