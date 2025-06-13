#include "PrecompileHeader.h"
#include "Ext_OldFilm.h"

#include <DirectX11_Base/Base_Windows.h>
#include "Ext_MeshComponentUnit.h"

void Ext_OldFilm::Start()
{
	// 기본 텍스쳐들 로드
	{
		Base_Directory Dir;
		Dir.MakePath("../Resource/FX/ScreenFX");
		std::vector<std::string> Paths = Dir.GetAllFile({ "png" });
		for (const std::string& FilePath : Paths)
		{
			Dir.SetPath(FilePath.c_str());
			std::string ExtensionName = Dir.GetExtension();
			std::string FileName = Dir.GetFileName();
			Textures.push_back(Ext_DirectXTexture::LoadTexture(FilePath.c_str()));
		}
	}

	OFData.OldFilmValue.x = 3.f;
	MaxIndex = static_cast<int>(Textures.size()) - 1;

	PostUnit = std::make_shared<Ext_MeshComponentUnit>();
	PostUnit->MeshComponentUnitInitialize("FullRect", "OldFilm");
	PostTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL);
}

void Ext_OldFilm::PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, float _DeltaTime)
{
	AccTime += _DeltaTime;
	if (AccTime >= 0.1f)
	{
		AccTime = 0.0f;
		++CurIndex;
		if (CurIndex > MaxIndex)
		{
			CurIndex = 0;
		}
	}

	PostUnit->GetBufferSetter().SetTexture(Textures[CurIndex], "DiffuseTex");
	PostUnit->GetBufferSetter().SetConstantBufferLink("OldFilmData", OFData);
	PostTarget->RenderTargetClear();
	PostTarget->RenderTargetSetting();

	PostUnit->Rendering(_DeltaTime);
	PostUnit->GetBufferSetter().AllTextureResourceReset();

	//_MainRenderTarget->RenderTargetClear();
	_MainRenderTarget->RenderTargetSetting();
	_MainRenderTarget->Merge(PostTarget);
}
