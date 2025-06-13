#pragma once
#include "Ext_ResourceManager.h"

// DepthStencil State를 관리하는 클래스
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

	// DepthStencilState 생성
	static std::shared_ptr<Ext_DirectXDepth> CreateDepthStencilState(const std::string_view& _Name, const D3D11_DEPTH_STENCIL_DESC& _Desc)
	{
		std::shared_ptr<Ext_DirectXDepth> NewDepth = Ext_DirectXDepth::CreateNameResource(_Name);
		NewDepth->CreateDepthStencilState(_Desc);

		return NewDepth;
	}

	COMPTR<ID3D11DepthStencilState>& GetDSS() { return DSS; }

protected:
	
private:
	void CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& _Value);
	void Setting(); // DSS를 GPU에 바인딩
	void SettingStencil();

	D3D11_DEPTH_STENCIL_DESC DepthStencilInfo = {}; // 뎁스스텐실스테이트 설명서
	COMPTR<ID3D11DepthStencilState> DSS = nullptr;		// 생성된 DSS 저장하는 포인터
	
};