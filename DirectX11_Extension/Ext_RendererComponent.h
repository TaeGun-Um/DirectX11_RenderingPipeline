#pragma once
#include "Ext_Component.h"

// ·»´õ¸µ ÄÄÆ÷³ÍÆ®
class Ext_RendererComponent : public Ext_Component
{
public:
	// constrcuter destructer
	Ext_RendererComponent();
	~Ext_RendererComponent();

	// delete Function
	Ext_RendererComponent(const Ext_RendererComponent& _Other) = delete;
	Ext_RendererComponent(Ext_RendererComponent&& _Other) noexcept = delete;
	Ext_RendererComponent& operator=(const Ext_RendererComponent& _Other) = delete;
	Ext_RendererComponent& operator=(Ext_RendererComponent&& _Other) noexcept = delete;

protected:
	
private:
	
};