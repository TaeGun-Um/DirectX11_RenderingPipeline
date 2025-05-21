#pragma once
#include "Ext_Component.h"

// ·»´õ¸µ ÄÄÆ÷³ÍÆ®
class Ext_MeshComponent : public Ext_Component
{
public:
	// constrcuter destructer
	Ext_MeshComponent();
	~Ext_MeshComponent();

	// delete Function
	Ext_MeshComponent(const Ext_MeshComponent& _Other) = delete;
	Ext_MeshComponent(Ext_MeshComponent&& _Other) noexcept = delete;
	Ext_MeshComponent& operator=(const Ext_MeshComponent& _Other) = delete;
	Ext_MeshComponent& operator=(Ext_MeshComponent&& _Other) noexcept = delete;

	// Getter, Setter

protected:
	virtual void Start() override;
	virtual void Update(float _DeltaTime) override {}
	virtual void Destroy() override {}

private:
	
};