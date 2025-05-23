#pragma once

class Ext_DirectXInputLayout
{
public:
	// constrcuter destructer
	Ext_DirectXInputLayout();
	~Ext_DirectXInputLayout();

	// delete Function
	Ext_DirectXInputLayout(const Ext_DirectXInputLayout& _Other) = delete;
	Ext_DirectXInputLayout(Ext_DirectXInputLayout&& _Other) noexcept = delete;
	Ext_DirectXInputLayout& operator=(const Ext_DirectXInputLayout& _Other) = delete;
	Ext_DirectXInputLayout& operator=(Ext_DirectXInputLayout&& _Other) noexcept = delete;
	
	void Relase();
	void CreateInputLayout(std::shared_ptr<class Ext_DirectXVertexBuffer> _VertexBuffer, std::shared_ptr<class Ext_DirectXVertexShader> _Shader);
	void InputLayoutSetting();

	COMPTR<ID3D11InputLayout>& GetInputLayout() { return InputLayout; }

protected:

private:
	COMPTR<ID3D11InputLayout> InputLayout = nullptr;

};