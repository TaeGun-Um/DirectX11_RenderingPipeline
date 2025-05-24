#pragma once
#include "Ext_ResourceManager.h"

// 렌더링 파이프라인 세팅 실시를 위한 클래스
class Ext_DirectXMaterial : public Ext_ResourceManager<Ext_DirectXMaterial>
{
	friend class Ext_MeshComponentUnit;

public:
	// constrcuter destructer
	Ext_DirectXMaterial() {};
	~Ext_DirectXMaterial() {};

	// delete Function
	Ext_DirectXMaterial(const Ext_DirectXMaterial& _Other) = delete;
	Ext_DirectXMaterial(Ext_DirectXMaterial&& _Other) noexcept = delete;
	Ext_DirectXMaterial& operator=(const Ext_DirectXMaterial& _Other) = delete;
	Ext_DirectXMaterial& operator=(Ext_DirectXMaterial&& _Other) noexcept = delete;

	// 머티리얼 생성
	static std::shared_ptr<class Ext_DirectXMaterial> CreateMaterial(const std::string_view& _Name)
	{
		std::shared_ptr<class Ext_DirectXMaterial> NewRes = Ext_ResourceManager<Ext_DirectXMaterial>::CreateNameResource(_Name);
		return NewRes;
	}

	// 머티리얼 세팅 실시
	void SetVertexShader(std::string_view _Name);
	void SetPixelShader(std::string_view _Name);
	void SetBlendState(std::string_view _Name);
	void SetDepthState(std::string_view _Name);
	void SetRasterizer(std::string_view _Name);

	// Getter, Setter
	std::shared_ptr<class Ext_DirectXVertexShader> GetVertexShader() { return VertexShader; }
	std::shared_ptr<class Ext_DirectXPixelShader> GetPixelShader() { return PixelShader; }

protected:
	
private:
	// 렌더링 파이프라인 세팅
	void MaterialSetting(); // 아래의 함수들 차례대로 실행
	void VertexShaderSetting();
	void HullShaderSetting();
	void TessellatorSetting();
	void DomainShaderSetting();
	void GeometryShaderSetting();
	void RasterizerSetting();
	void PixelShaderSetting();
	void OutputMergerSetting();

	std::shared_ptr<class Ext_DirectXVertexShader> VertexShader; // 버텍스 셰이더 저장
	std::shared_ptr<class Ext_DirectXPixelShader> PixelShader; // 픽셀 셰이더 저장
	//std::shared_ptr<class Ext_DirectXDepthState> DepthStatePtr;
	
	
};