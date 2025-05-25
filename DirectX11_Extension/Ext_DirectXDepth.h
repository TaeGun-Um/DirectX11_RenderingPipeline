#pragma once

class Ext_DirectXDepth
{
public:
	// constrcuter destructer
	Ext_DirectXDepth() {};
	~Ext_DirectXDepth() {};

	// delete Function
	Ext_DirectXDepth(const Ext_DirectXDepth& _Other) = delete;
	Ext_DirectXDepth(Ext_DirectXDepth&& _Other) noexcept = delete;
	Ext_DirectXDepth& operator=(const Ext_DirectXDepth& _Other) = delete;
	Ext_DirectXDepth& operator=(Ext_DirectXDepth&& _Other) noexcept = delete;

protected:
	
private:
	
};