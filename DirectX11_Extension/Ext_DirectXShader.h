#pragma once

class Ext_DirectXShader
{
public:
	// constrcuter destructer
	Ext_DirectXShader() {};
	~Ext_DirectXShader() {};

	// delete Function
	Ext_DirectXShader(const Ext_DirectXShader& _Other) = delete;
	Ext_DirectXShader(Ext_DirectXShader&& _Other) noexcept = delete;
	Ext_DirectXShader& operator=(const Ext_DirectXShader& _Other) = delete;
	Ext_DirectXShader& operator=(Ext_DirectXShader&& _Other) noexcept = delete;

protected:
	
private:
	
};