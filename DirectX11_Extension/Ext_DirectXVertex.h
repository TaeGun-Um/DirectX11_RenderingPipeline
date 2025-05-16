#pragma once

class Ext_DirectXVertex
{
public:
	// constrcuter destructer
	Ext_DirectXVertex() {};
	~Ext_DirectXVertex() {};

	// delete Function
	Ext_DirectXVertex(const Ext_DirectXVertex& _Other) = delete;
	Ext_DirectXVertex(Ext_DirectXVertex&& _Other) noexcept = delete;
	Ext_DirectXVertex& operator=(const Ext_DirectXVertex& _Other) = delete;
	Ext_DirectXVertex& operator=(Ext_DirectXVertex&& _Other) noexcept = delete;

protected:
	
private:
	
};

