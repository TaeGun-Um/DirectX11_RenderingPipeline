#pragma once

class Ext_DirectXMaterial
{
public:
	// constrcuter destructer
	Ext_DirectXMaterial() {};
	~Ext_DirectXMaterial() {};

	// delete Function
	Ext_DirectXMaterial(const Ext_DirectXMaterial& _Other) = delete;
	Ext_DirectXMaterial(Ext_DirectXMaterial&& _Other) noexcept = delete;
	Ext_DirectXMaterial& operator=(const Ext_DirectXMaterial& _Other) = delete;
	Ext_DirectXMaterial& operator=(Ext_DirectXMaterial&& _Other) noexcept = delete;

protected:
	
private:
	
};