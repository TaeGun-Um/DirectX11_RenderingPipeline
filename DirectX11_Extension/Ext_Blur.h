#pragma once
#include "Ext_PostProcess.h"

class Ext_Blur : public Ext_PostProcess
{
public:
	// constrcuter destructer
	Ext_Blur() {}
	~Ext_Blur() {}

	// delete Function
	Ext_Blur(const Ext_Blur& _Other) = delete;
	Ext_Blur(Ext_Blur&& _Other) noexcept = delete;
	Ext_Blur& operator=(const Ext_Blur& _Other) = delete;
	Ext_Blur& operator=(Ext_Blur&& _Other) noexcept = delete;

protected:
	void Start() override;
	void PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, float _DeltaTime) override;
	
private:

};