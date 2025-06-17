#include "PrecompileHeader.h"
#include "Ext_TextureTest.h"

#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Extension/Ext_DirectXTexture.h>
#include "Ext_MeshComponentUnit.h"

void Ext_TextureTest::Start()
{
	PostUnit = std::make_shared<Ext_MeshComponentUnit>();
	PostUnit->MeshComponentUnitInitialize("FullRect", MaterialType::RenderTargetMerge);
	PostTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL);
}

void Ext_TextureTest::PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, float _DeltaTime)
{
	std::shared_ptr<Ext_DirectXTexture> Tex = Ext_DirectXTexture::Find("SkyBox");

	PostUnit->GetBufferSetter().SetTexture(Tex, "DiffuseTex");
	PostTarget->RenderTargetClear();
	PostTarget->RenderTargetSetting();

	PostUnit->Rendering(_DeltaTime);
	PostUnit->GetBufferSetter().AllTextureResourceReset();

	//_MainRenderTarget->RenderTargetClear();
	_MainRenderTarget->RenderTargetSetting();
	_MainRenderTarget->Merge(PostTarget);
}
