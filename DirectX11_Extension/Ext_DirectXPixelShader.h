#include "Ext_ResourceManager.h"
#include "Ext_DirectXShader.h"

// DirectX의 픽셀 셰이더 컴파일 및 정보 저장을 위한 클래스
class Ext_DirectXPixelShader : public Ext_ResourceManager<Ext_DirectXPixelShader>, public Ext_DirectXShader
{
	friend class Ext_DirectXMaterial;

public:
	// constrcuter destructer
	Ext_DirectXPixelShader();
	~Ext_DirectXPixelShader() {}

	// delete Function
	Ext_DirectXPixelShader(const Ext_DirectXPixelShader& _Other) = delete;
	Ext_DirectXPixelShader(Ext_DirectXPixelShader&& _Other) noexcept = delete;
	Ext_DirectXPixelShader& operator=(const Ext_DirectXPixelShader& _Other) = delete;
	Ext_DirectXPixelShader& operator=(Ext_DirectXPixelShader&& _Other) noexcept = delete;

	// 픽셀 셰이더 생성 및 정보 저장
	static std::shared_ptr<Ext_DirectXPixelShader> LoadPixelShader(std::string_view _Path, const std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0)
	{
		std::shared_ptr<Ext_DirectXPixelShader> NewShader = Ext_DirectXPixelShader::CreateNameResource(_EntryPoint);
		NewShader->CreatePixelShader(_Path, _EntryPoint, _VersionHigh, _VersionLow);
		return NewShader;
	}

protected:

private:
	void CreatePixelShader(std::string_view _Path, std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0);
	void PixelShaderSetting(); // PixelShaderSetting() 호출
	
	COMPTR<ID3D11PixelShader> PixelShader = nullptr; // 생성한 픽셀 셰이더 정보 저장용
};