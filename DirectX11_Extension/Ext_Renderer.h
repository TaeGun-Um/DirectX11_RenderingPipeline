#pragma once
#include "Ext_Component.h"

class Ext_Renderer : public Ext_Component
{
public:
	// constrcuter destructer
	Ext_Renderer();
	~Ext_Renderer();

	// delete Function
	Ext_Renderer(const Ext_Renderer& _Other) = delete;
	Ext_Renderer(Ext_Renderer&& _Other) noexcept = delete;
	Ext_Renderer& operator=(const Ext_Renderer& _Other) = delete;
	Ext_Renderer& operator=(Ext_Renderer&& _Other) noexcept = delete;

protected:
	
private:
	
};