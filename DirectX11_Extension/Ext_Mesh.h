#pragma once

class Ext_Mesh
{
public:
	// constrcuter destructer
	Ext_Mesh();
	~Ext_Mesh();

	// delete Function
	Ext_Mesh(const Ext_Mesh& _Other) = delete;
	Ext_Mesh(Ext_Mesh&& _Other) noexcept = delete;
	Ext_Mesh& operator=(const Ext_Mesh& _Other) = delete;
	Ext_Mesh& operator=(Ext_Mesh&& _Other) noexcept = delete;

protected:
	
private:
	
};