#pragma once

class Ext_DirectXRasterizer
{
public:
	// constrcuter destructer
	Ext_DirectXRasterizer() {};
	~Ext_DirectXRasterizer() {};

	// delete Function
	Ext_DirectXRasterizer(const Ext_DirectXRasterizer& _Other) = delete;
	Ext_DirectXRasterizer(Ext_DirectXRasterizer&& _Other) noexcept = delete;
	Ext_DirectXRasterizer& operator=(const Ext_DirectXRasterizer& _Other) = delete;
	Ext_DirectXRasterizer& operator=(Ext_DirectXRasterizer&& _Other) noexcept = delete;

protected:
	
private:
	
};