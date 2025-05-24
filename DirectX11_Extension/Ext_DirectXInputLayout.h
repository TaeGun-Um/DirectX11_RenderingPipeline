#pragma once

// ���ؽ� ����, ���ؽ� ���̴��� ���� ��ǲ ���̾ƿ��� �����Ͽ� ������ �����ϴ� Ŭ����
class Ext_DirectXInputLayout
{
	friend class Ext_MeshComponentUnit;

public:
	// constrcuter destructer
	Ext_DirectXInputLayout() {}
	~Ext_DirectXInputLayout() {}

	// delete Function
	Ext_DirectXInputLayout(const Ext_DirectXInputLayout& _Other) = delete;
	Ext_DirectXInputLayout(Ext_DirectXInputLayout&& _Other) noexcept = delete;
	Ext_DirectXInputLayout& operator=(const Ext_DirectXInputLayout& _Other) = delete;
	Ext_DirectXInputLayout& operator=(Ext_DirectXInputLayout&& _Other) noexcept = delete;
	
	// Getter
	COMPTR<ID3D11InputLayout>& GetInputLayout() { return InputLayout; }

protected:

private:
	void CreateInputLayout(std::shared_ptr<class Ext_DirectXVertexBuffer> _VertexBuffer, std::shared_ptr<class Ext_DirectXVertexShader> _Shader); // CreateInputLayout() ȣ��
	void Relase();					// InputLayout ���� ��, 1ȸ �ǽ�
	void InputLayoutSetting();  // IASetInputLayout() ȣ��

	COMPTR<ID3D11InputLayout> InputLayout = nullptr; // ������ ��ǲ ���̾ƿ� ���� ����

};