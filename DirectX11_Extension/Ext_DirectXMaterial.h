#pragma once
#include "Ext_ResourceManager.h"

class Ext_DirectXMaterial : public Ext_ResourceManager<Ext_DirectXMaterial>
{
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

	// 렌더링 파이프라인 세팅
	void MaterialSetting();
	void VertexShaderSetting();
	void HullShaderSetting();
	void TessellatorSetting();
	void DomainShaderSetting();
	void GeometryShaderSetting();
	void RasterizerSetting();
	void PixelShaderSetting();
	void OutputMergerSetting();


	// Getter, Setter
	std::shared_ptr<class Ext_DirectXVertexBuffer> GetVertexBuffer() { return VertexBuffer; }
	std::shared_ptr<class Ext_DirectXIndexBuffer> GetIndexBuffer() { return IndexBuffer; }
	std::shared_ptr<class Ext_DirectXVertexShader> GetVertexShader() { return VertexShader; }
	std::shared_ptr<class Ext_DirectXPixelShader> GetPixelShader() { return PixelShader; }

protected:
	
private:
	std::shared_ptr<class Ext_DirectXVertexBuffer> VertexBuffer;
	std::shared_ptr<class Ext_DirectXIndexBuffer> IndexBuffer;
	std::shared_ptr<class Ext_DirectXVertexShader> VertexShader;
	std::shared_ptr<class Ext_DirectXPixelShader> PixelShader;
	//std::shared_ptr<class Ext_DirectXDepthState> DepthStatePtr;
	
	
};