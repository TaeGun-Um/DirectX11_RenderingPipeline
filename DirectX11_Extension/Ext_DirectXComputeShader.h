#include "Ext_ResourceManager.h"
#include "Ext_DirectXShader.h"

// DirectX의 컴퓨트 셰이더 컴파일 및 정보 저장을 위한 클래스
class Ext_DirectXComputeShader : public Ext_ResourceManager<Ext_DirectXComputeShader>, public Ext_DirectXShader
{
	friend class Ext_DirectXMaterial;

public:
	// constrcuter destructer
	Ext_DirectXComputeShader();
	~Ext_DirectXComputeShader() {}

	// delete Function
	Ext_DirectXComputeShader(const Ext_DirectXComputeShader& _Other) = delete;
	Ext_DirectXComputeShader(Ext_DirectXComputeShader&& _Other) noexcept = delete;
	Ext_DirectXComputeShader& operator=(const Ext_DirectXComputeShader& _Other) = delete;
	Ext_DirectXComputeShader& operator=(Ext_DirectXComputeShader&& _Other) noexcept = delete;

	// 컴퓨트 셰이더 생성 및 정보 저장
	static std::shared_ptr<Ext_DirectXComputeShader> LoadComputeShader(std::string_view _Path, const std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0)
	{
		std::shared_ptr<Ext_DirectXComputeShader> NewShader = Ext_DirectXComputeShader::CreateNameResource(_EntryPoint);
		NewShader->CreateComputeShader(_Path, _EntryPoint, _VersionHigh, _VersionLow);
		return NewShader;
	}

protected:

private:
	void CreateComputeShader(std::string_view _Path, std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0);
	void ComputeShaderSetting(); // CSSetShader() 호출

	COMPTR<ID3D11ComputeShader> ComputeShader = nullptr; // 생성한 컴퓨트 셰이더 정보 저장용
};