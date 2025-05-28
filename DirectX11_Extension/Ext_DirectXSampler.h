#pragma once
#include "Ext_ResourceManager.h"

// DirectX11의 Sampler 속성을 관리하는 클래스
class Ext_DirectXSampler : public Ext_ResourceManager<Ext_DirectXSampler>
{
public:
	// constrcuter destructer
	Ext_DirectXSampler() {}
	~Ext_DirectXSampler() {}

	// delete Function
	Ext_DirectXSampler(const Ext_DirectXSampler& _Other) = delete;
	Ext_DirectXSampler(Ext_DirectXSampler&& _Other) noexcept = delete;
	Ext_DirectXSampler& operator=(const Ext_DirectXSampler& _Other) = delete;
	Ext_DirectXSampler& operator=(Ext_DirectXSampler&& _Other) noexcept = delete;

	// Sampler 생성
	static std::shared_ptr<Ext_DirectXSampler> CreateSampler(std::string_view _Name, const D3D11_SAMPLER_DESC& Desc)
	{
		std::shared_ptr<Ext_DirectXSampler> NewSampler = Ext_ResourceManager::CreateNameResource(_Name);
		NewSampler->CreateSampler(Desc);

		return NewSampler;
	}

	static std::shared_ptr<Ext_DirectXSampler> SamplerSetting(std::string_view _Name, const D3D11_SAMPLER_DESC& Desc)
	{
		std::shared_ptr<Ext_DirectXSampler> NewSampler = Ext_ResourceManager::Find(_Name);

		if (nullptr == NewSampler)
		{
			MsgAssert("존재하지 않는 샘플러의 옵션을 변경하려고 했습니다.");
			return nullptr;
		}

		NewSampler->CreateSampler(Desc);

		return NewSampler;
	}

	void VSSetting(UINT _Slot);
	void PSSetting(UINT _Slot);

protected:
	
private:
	COMPTR<ID3D11SamplerState> SmplerState = nullptr; // 생성된 샘플러 저장
	D3D11_SAMPLER_DESC SamplerInfo; // 샘플러 설명서

	void CreateSampler(const D3D11_SAMPLER_DESC& _Desc);
	
};