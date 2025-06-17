#pragma once
#include "Ext_PostProcess.h"

class Ext_TextureTest : public Ext_PostProcess
{
public:
	// constrcuter destructer
	Ext_TextureTest() {}
	~Ext_TextureTest() {}

	// delete Function
	Ext_TextureTest(const Ext_TextureTest& _Other) = delete;
	Ext_TextureTest(Ext_TextureTest&& _Other) noexcept = delete;
	Ext_TextureTest& operator=(const Ext_TextureTest& _Other) = delete;
	Ext_TextureTest& operator=(Ext_TextureTest&& _Other) noexcept = delete;

protected:
	void Start() override;
	void PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, std::shared_ptr<class Ext_Camera> _Camera, float _DeltaTime) override;
	
private:
	std::vector<std::shared_ptr<class Ext_DirectXTexture>> Textures;
	std::shared_ptr<class Ext_Transform> Transform;

};