#pragma once

class Ext_DirectXSampler
{
public:
	// constrcuter destructer
	Ext_DirectXSampler() {};
	~Ext_DirectXSampler() {};

	// delete Function
	Ext_DirectXSampler(const Ext_DirectXSampler& _Other) = delete;
	Ext_DirectXSampler(Ext_DirectXSampler&& _Other) noexcept = delete;
	Ext_DirectXSampler& operator=(const Ext_DirectXSampler& _Other) = delete;
	Ext_DirectXSampler& operator=(Ext_DirectXSampler&& _Other) noexcept = delete;

protected:
	
private:
	
};