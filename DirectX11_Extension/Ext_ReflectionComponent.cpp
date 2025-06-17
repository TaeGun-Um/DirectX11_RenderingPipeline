#include "PrecompileHeader.h"
#include "Ext_ReflectionComponent.h"

#include <DirectX11_Base/Base_Directory.h>

#include "Ext_ScreenShoot.h"
#include "Ext_DirectXTexture.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_Transform.h"
#include "Ext_Scene.h"
#include "Ext_Camera.h"

std::shared_ptr<Ext_DirectXRenderTarget> Ext_ReflectionComponent::CaptureTarget = nullptr;

void Ext_ReflectionComponent::ReflectionInitialize(std::string_view _CaptureTextureName, const float4& _Scale/* = float4(128, 128)*/)
{
	Base_Directory Dir;
	Dir.MakePath("../Resource/FX/ReflectionTexture");
	std::string Path = Dir.GetPath();

	if (nullptr == Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_Forword.png")))
	{
		if (0 == _Scale.x || 0 == _Scale.y)
		{
			MsgAssert("ReflectionProbe : 캡쳐할 텍스쳐의 크기가 0 입니다");
			return;
		}

		if (nullptr == CaptureTarget)
		{
			CaptureTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UNORM, _Scale, float4::ZERONULL);
		}

		float4 CenterPos = GetTransform()->GetWorldPosition();
		float4 CenterRot = float4::ZERO;

		// Forward
		GetOwnerScene().lock()->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot, float4(900, 900));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(GetOwnerScene().lock()->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Forword.png"));

		// Back
		GetOwnerScene().lock()->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot + float4(0, 180, 0), float4(900, 900));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(GetOwnerScene().lock()->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Back.png"));

		// Right
		GetOwnerScene().lock()->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot + float4(0, 90, 0), float4(900, 900));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(GetOwnerScene().lock()->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Right.png"));

		// Left
		GetOwnerScene().lock()->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot + float4(0, -90, 0), float4(900, 900));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(GetOwnerScene().lock()->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Left.png"));

		// Top
		GetOwnerScene().lock()->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot + float4(-90, 0, 0), float4(900, 900));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(GetOwnerScene().lock()->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Top.png"));

		// Bottom
		GetOwnerScene().lock()->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot + float4(90, 0, 0), float4(900, 900));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(GetOwnerScene().lock()->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Bottom.png"));
		CaptureTarget->RenderTargetClear();

		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Forword.PNG");
		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Back.PNG");
		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Right.PNG");
		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Left.PNG");
		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Top.PNG");
		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Bottom.PNG");
	}

	CubeTexture = Ext_DirectXTexture::Find(_CaptureTextureName);

	if (nullptr == CubeTexture)
	{
		std::vector<std::shared_ptr<Ext_DirectXTexture>> CubeTextures;
		CubeTextures.reserve(6);

		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_FORWARD.PNG")));
		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_BACK.PNG")));
		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_RIGHT.PNG")));
		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_LEFT.PNG")));
		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_TOP.PNG")));
		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_BOT.PNG")));

		//ReflectionCubeTexture = Ext_DirectXTexture::Create(_CaptureTextureName, CubeTextures);
	}
}
