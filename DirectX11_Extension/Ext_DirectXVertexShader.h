#pragma once
#include "Ext_ResourceManager.h"
#include "Ext_DirectXShader.h"

// DirectX�� ���ؽ� ���̴� ������ �� ���� ������ ���� Ŭ����
class Ext_DirectXVertexShader : public Ext_ResourceManager<Ext_DirectXVertexShader>, public Ext_DirectXShader
{
	friend class Ext_DirectXMaterial;

public:
	// constrcuter destructer
	Ext_DirectXVertexShader();
	~Ext_DirectXVertexShader() {}

	// delete Function
	Ext_DirectXVertexShader(const Ext_DirectXVertexShader& _Other) = delete;
	Ext_DirectXVertexShader(Ext_DirectXVertexShader&& _Other) noexcept = delete;
	Ext_DirectXVertexShader& operator=(const Ext_DirectXVertexShader& _Other) = delete;
	Ext_DirectXVertexShader& operator=(Ext_DirectXVertexShader&& _Other) noexcept = delete;

	// ���ؽ� ���̴� ���� �� ���� ����
	static std::shared_ptr<Ext_DirectXVertexShader> LoadVertexShader(std::string_view _Path, const std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0)
	{
		std::shared_ptr<Ext_DirectXVertexShader> NewShader = Ext_DirectXVertexShader::CreateNameResource(_EntryPoint);
		NewShader->CreateVertexShader(_Path, _EntryPoint, _VersionHigh, _VersionLow);
		return NewShader;
	}

protected:
	
private:
	void CreateVertexShader(std::string_view _Path, std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0);
	void VertexShaderSetting(); // VertexShaderSetting() ȣ��
	
	COMPTR<ID3D11VertexShader> VertexShader = nullptr; // ������ ���ؽ� ���̴� ���� �����
};