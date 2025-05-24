#pragma once
#include "Ext_ResourceManager.h"

// ������ ���������� ���� �ǽø� ���� Ŭ����
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

	// ��Ƽ���� ����
	static std::shared_ptr<class Ext_DirectXMaterial> CreateMaterial(const std::string_view& _Name)
	{
		std::shared_ptr<class Ext_DirectXMaterial> NewRes = Ext_ResourceManager<Ext_DirectXMaterial>::CreateNameResource(_Name);
		return NewRes;
	}

	// ��Ƽ���� ���� �ǽ�
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
	// ������ ���������� ����
	void MaterialSetting(); // �Ʒ��� �Լ��� ���ʴ�� ����
	void VertexShaderSetting();
	void HullShaderSetting();
	void TessellatorSetting();
	void DomainShaderSetting();
	void GeometryShaderSetting();
	void RasterizerSetting();
	void PixelShaderSetting();
	void OutputMergerSetting();

	std::shared_ptr<class Ext_DirectXVertexShader> VertexShader; // ���ؽ� ���̴� ����
	std::shared_ptr<class Ext_DirectXPixelShader> PixelShader; // �ȼ� ���̴� ����
	//std::shared_ptr<class Ext_DirectXDepthState> DepthStatePtr;
	
	
};