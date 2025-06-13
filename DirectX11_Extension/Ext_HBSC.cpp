#include "PrecompileHeader.h"
#include "Ext_HBSC.h"

#include <DirectX11_Base/Base_Windows.h>
#include "Ext_MeshComponentUnit.h"

void Ext_HBSC::Start()
{
	//PostUnit = std::make_shared<Ext_MeshComponentUnit>();
	//PostUnit->MeshComponentUnitInitialize("FullRect", "HBSC");
	//PostTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL);
}

void Ext_HBSC::PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, float _DeltaTime)
{
	//PostTarget->RenderTargetClear();
	//PostTarget->RenderTargetSetting();

	//PostUnit->Rendering(_DeltaTime);
	//PostUnit->GetBufferSetter().AllTextureResourceReset();

	//_MainRenderTarget->RenderTargetClear();
	//_MainRenderTarget->Merge(PostTarget);
}
