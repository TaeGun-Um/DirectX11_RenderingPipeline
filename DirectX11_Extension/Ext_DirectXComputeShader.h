#include "Ext_ResourceManager.h"
#include "Ext_DirectXShader.h"

class Ext_DirectXComputeShader : public Ext_ResourceManager<Ext_DirectXComputeShader>, public Ext_DirectXShader
{
public:
	// constrcuter destructer
	Ext_DirectXComputeShader();
	~Ext_DirectXComputeShader();

	// delete Function
	Ext_DirectXComputeShader(const Ext_DirectXComputeShader& _Other) = delete;
	Ext_DirectXComputeShader(Ext_DirectXComputeShader&& _Other) noexcept = delete;
	Ext_DirectXComputeShader& operator=(const Ext_DirectXComputeShader& _Other) = delete;
	Ext_DirectXComputeShader& operator=(Ext_DirectXComputeShader&& _Other) noexcept = delete;

	static std::shared_ptr<Ext_DirectXComputeShader> LoadComputeShader(std::string_view _Path, const std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0)
	{
		std::shared_ptr<Ext_DirectXComputeShader> NewShader = Ext_DirectXComputeShader::CreateNameResource(_EntryPoint);
		NewShader->CreateComputeShader(_Path, _EntryPoint, _VersionHigh, _VersionLow);
		return NewShader;
	}

	void ComputeShaderSetting();

protected:

private:
	void CreateComputeShader(std::string_view _Path, std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0);
	
	COMPTR<ID3D11ComputeShader> ComputeShader = nullptr;
};