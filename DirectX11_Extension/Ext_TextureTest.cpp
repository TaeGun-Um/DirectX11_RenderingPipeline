#include "PrecompileHeader.h"
#include "Ext_TextureTest.h"

#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Extension/Ext_DirectXTexture.h>
#include <DirectX11_Extension/Ext_Camera.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include "Ext_MeshComponentUnit.h"

void Ext_TextureTest::Start()
{
	PostUnit = std::make_shared<Ext_MeshComponentUnit>();
	PostUnit->MeshComponentUnitInitialize("FullBox", "SkyBox");
	PostTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL);
	Transform = std::make_shared<Ext_Transform>();
}

void Ext_TextureTest::PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, std::shared_ptr<class Ext_Camera> _Camera, float _DeltaTime)
{
	std::shared_ptr<Ext_DirectXTexture> Tex = Ext_DirectXTexture::Find("SkyBox");

	PostTarget->RenderTargetClear();
	PostTarget->RenderTargetSetting();

	float4 CamPos = _Camera->GetTransform()->GetLocalPosition();
	Transform->SetLocalPosition(CamPos);
	Transform->SetCameraMatrix(_Camera->GetTransform()->GetLocalPosition(), _Camera->GetViewMatrix(), _Camera->GetProjectionMatrix());

	PostUnit->GetBufferSetter().SetConstantBufferLink("TransformData", *Transform->GetTransformData());
	PostUnit->GetBufferSetter().SetTexture(Tex, "CubeMapTex");
	PostUnit->Rendering(_DeltaTime);
	PostUnit->GetBufferSetter().AllTextureResourceReset();

	//_MainRenderTarget->RenderTargetClear();

	//_MainRenderTarget->RenderTargetSetting();
	PostTarget->Merge(_MainRenderTarget);
}
