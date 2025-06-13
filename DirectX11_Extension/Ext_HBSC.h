#pragma once
#include "Ext_PostProcess.h"

class Ext_HBSC : public Ext_PostProcess
{
public:
	// constrcuter destructer
	Ext_HBSC() {}
	~Ext_HBSC() {}

	// delete Function
	Ext_HBSC(const Ext_HBSC& _Other) = delete;
	Ext_HBSC(Ext_HBSC&& _Other) noexcept = delete;
	Ext_HBSC& operator=(const Ext_HBSC& _Other) = delete;
	Ext_HBSC& operator=(Ext_HBSC&& _Other) noexcept = delete;

protected:
	void Start() override;
	void PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, float _DeltaTime) override;
	
private:
	
};