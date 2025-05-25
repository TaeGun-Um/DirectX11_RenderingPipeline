#pragma once
#include "Ext_ResourceManager.h"
#include "Ext_DirectXShader.h"

// DirectX의 버텍스 셰이더 컴파일 및 정보 저장을 위한 클래스
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

	// 버텍스 셰이더 생성 및 정보 저장
	static std::shared_ptr<Ext_DirectXVertexShader> LoadVertexShader(std::string_view _Path, const std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0)
	{
		std::shared_ptr<Ext_DirectXVertexShader> NewShader = Ext_DirectXVertexShader::CreateNameResource(_EntryPoint);
		NewShader->CreateVertexShader(_Path, _EntryPoint, _VersionHigh, _VersionLow);
		return NewShader;
	}

protected:
	
private:
	void CreateVertexShader(std::string_view _Path, std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0);
	void VertexShaderSetting(); // VertexShaderSetting() 호출
	
	COMPTR<ID3D11VertexShader> VertexShader = nullptr; // 생성한 버텍스 셰이더 정보 저장용
};