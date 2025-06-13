#pragma once
#include "Ext_PostProcess.h"

class Ext_Distortion : public Ext_PostProcess
{
public:
	// constrcuter destructer
	Ext_Distortion() {}
	~Ext_Distortion() {}

	// delete Function
	Ext_Distortion(const Ext_Distortion& _Other) = delete;
	Ext_Distortion(Ext_Distortion&& _Other) noexcept = delete;
	Ext_Distortion& operator=(const Ext_Distortion& _Other) = delete;
	Ext_Distortion& operator=(Ext_Distortion&& _Other) noexcept = delete;

protected:
	void Start() override;
	void PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, float _DeltaTime) override;
	
private:
	
};