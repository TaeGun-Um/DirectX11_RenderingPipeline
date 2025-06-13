#include "PrecompileHeader.h"
#include "Ext_Distortion.h"

#include <DirectX11_Base/Base_Windows.h>
#include "Ext_MeshComponentUnit.h"

void Ext_Distortion::Start()
{
	PostUnit = std::make_shared<Ext_MeshComponentUnit>();
	PostUnit->MeshComponentUnitInitialize("FullRect", "Distortion");
	PostTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL);	
}

void Ext_Distortion::PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, float _DeltaTime)
{
	PostTarget->RenderTargetClear();
	PostTarget->RenderTargetSetting();

	FData.ScreenSize = Base_Windows::GetScreenSize();
	FData.AccTime += _DeltaTime;

	PostUnit->GetBufferSetter().SetConstantBufferLink("FrameData", FData);
	PostUnit->GetBufferSetter().SetTexture(_MainRenderTarget->GetTexture(0), "DiffuseTex");
	PostUnit->Rendering(_DeltaTime);
	PostUnit->GetBufferSetter().AllTextureResourceReset();

	_MainRenderTarget->RenderTargetClear();
	_MainRenderTarget->Merge(PostTarget);
}
