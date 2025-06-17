#pragma once
#include "Ext_Component.h"

class Ext_ReflectionComponent : public Ext_Component
{
public:
	// constrcuter destructer
	Ext_ReflectionComponent() {}
	~Ext_ReflectionComponent() {}

	// delete Function
	Ext_ReflectionComponent(const Ext_ReflectionComponent& _Other) = delete;
	Ext_ReflectionComponent(Ext_ReflectionComponent&& _Other) noexcept = delete;
	Ext_ReflectionComponent& operator=(const Ext_ReflectionComponent& _Other) = delete;
	Ext_ReflectionComponent& operator=(Ext_ReflectionComponent&& _Other) noexcept = delete;

	void ReflectionInitialize(std::string_view _CaptureTextureName, const float4& _Scale = float4(128, 128));

protected:
	
private:
	static std::shared_ptr<class Ext_DirectXRenderTarget> CaptureTarget;
	std::shared_ptr<class Ext_DirectXTexture> CubeTexture = nullptr;
	
};