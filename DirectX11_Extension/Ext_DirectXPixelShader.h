#include "Ext_ResourceManager.h"
#include "Ext_DirectXShader.h"

class Ext_DirectXPixelShader : public Ext_ResourceManager<Ext_DirectXPixelShader>, public Ext_DirectXShader
{
public:
	// constrcuter destructer
	Ext_DirectXPixelShader();
	~Ext_DirectXPixelShader();

	// delete Function
	Ext_DirectXPixelShader(const Ext_DirectXPixelShader& _Other) = delete;
	Ext_DirectXPixelShader(Ext_DirectXPixelShader&& _Other) noexcept = delete;
	Ext_DirectXPixelShader& operator=(const Ext_DirectXPixelShader& _Other) = delete;
	Ext_DirectXPixelShader& operator=(Ext_DirectXPixelShader&& _Other) noexcept = delete;

	static std::shared_ptr<Ext_DirectXPixelShader> LoadPixelShader(std::string_view _Path, const std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0)
	{
		std::shared_ptr<Ext_DirectXPixelShader> NewShader = Ext_DirectXPixelShader::CreateNameResource(_EntryPoint);
		NewShader->CreatePixelShader(_Path, _EntryPoint, _VersionHigh, _VersionLow);
		return NewShader;
	}

	void PixelShaderSetting();


	COMPTR<ID3D11PixelShader>& GetPixelShader()// юс╫ц
	{
		return PixelShader;
	}


protected:

private:
	void CreatePixelShader(std::string_view _Path, std::string_view _EntryPoint, UINT _VersionHigh = 5, UINT _VersionLow = 0);
	
	COMPTR<ID3D11PixelShader> PixelShader = nullptr;
};