#pragma once

struct Setter
{
	std::string Name;
	// class GameEngineShader* ParentShader;
	int BindPoint = -1; // b0 t0 같은 몇번째 슬롯에 세팅되어야 하는지에 대한 정보.
	std::shared_ptr<class Ext_DirectXConstantBuffer> Res;
	const void* CPUData;
	UINT CPUDataSize;

	void Setting();
};

// DirectX에 필요한 Resource들을 로드하는 클래스
class Ext_DirectXResourceLoader
{
public:
	// delete Function
	Ext_DirectXResourceLoader(const Ext_DirectXResourceLoader& _Other) = delete;
	Ext_DirectXResourceLoader(Ext_DirectXResourceLoader&& _Other) noexcept = delete;
	Ext_DirectXResourceLoader& operator=(const Ext_DirectXResourceLoader& _Other) = delete;
	Ext_DirectXResourceLoader& operator=(Ext_DirectXResourceLoader&& _Other) noexcept = delete;

	static void Initialize();

	static COMPTR<ID3D11VertexShader>& GetVertexShader() { return BaseVertexShader; }
	static COMPTR<ID3D11PixelShader>& GetPixelShader() { return BasePixelShader; }
	static COMPTR<ID3D11InputLayout>& GetInputLayout() { return InputLayout; }

protected:
	
private:
	static void MakeVertex();
	static void MakeSampler();
	static void MakeBlend();
	static void MakeDepth();
	static void ShaderCompile();
	static void MakeRasterizer();
	static void MakeMaterial();

	static COMPTR<ID3D11VertexShader> BaseVertexShader;
	static COMPTR<ID3D11PixelShader> BasePixelShader;
	static COMPTR<ID3D11InputLayout> InputLayout;

	static std::multimap<std::string, Setter> ConstantBufferSetters;
};