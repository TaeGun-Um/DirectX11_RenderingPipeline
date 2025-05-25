#pragma once

// 버텍스 버퍼, 버텍스 셰이더를 통해 인풋 레이아웃을 생성하여 정보를 저장하는 클래스
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
	void CreateInputLayout(std::shared_ptr<class Ext_DirectXVertexBuffer> _VertexBuffer, std::shared_ptr<class Ext_DirectXVertexShader> _Shader); // CreateInputLayout() 호출
	void Relase();					// InputLayout 생성 전, 1회 실시
	void InputLayoutSetting();  // IASetInputLayout() 호출

	COMPTR<ID3D11InputLayout> InputLayout = nullptr; // 생성된 인풋 레이아웃 정보 저장

};