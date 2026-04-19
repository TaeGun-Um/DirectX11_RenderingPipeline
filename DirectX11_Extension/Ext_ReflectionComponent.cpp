#include "PrecompileHeader.h"
#include "Ext_ReflectionComponent.h"

#include <DirectX11_Base/Base_Directory.h>

#include "Ext_Actor.h"
#include "Ext_MeshComponent.h"
#include "Ext_ScreenShoot.h"
#include "Ext_DirectXTexture.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_Transform.h"
#include "Ext_Scene.h"
#include "Ext_Camera.h"

int Ext_ReflectionComponent::AttachCounter = 0;

std::shared_ptr<Ext_ReflectionComponent> Ext_ReflectionComponent::AttachTo(std::shared_ptr<Ext_Actor> _Owner, std::shared_ptr<Ext_MeshComponent> _Mesh, const float4& _CaptureSize /*= float4(512, 512)*/)
{
	if (nullptr == _Owner || nullptr == _Mesh)
	{
		MsgAssert("ReflectionComponent::AttachTo: nullptr owner or mesh");
		return nullptr;
	}

	std::shared_ptr<Ext_ReflectionComponent> Reflection = std::make_shared<Ext_ReflectionComponent>();
	std::string Name = "TestReflection" + std::to_string(AttachCounter++);

	_Mesh->SetSampler("CubeMapSampler", "CubeMapSampler");
	Reflection->ReflectionInitialize(_Owner, Name, _CaptureSize);
	_Mesh->SetTexture(Reflection->GetReflectionCubeTexture(), "ReflectionTexture");

	return Reflection;
}

void Ext_ReflectionComponent::ReflectionInitialize(std::shared_ptr<class Ext_Actor> _Owner, std::string_view _CaptureTextureName, const float4& _Scale/* = float4(128, 128)*/)
{
	Base_Directory Dir;
	Dir.MakePath("../Resource/FX/ReflectionTexture");
	std::string Path = Dir.GetPath();

	std::shared_ptr<Ext_DirectXRenderTarget> CaptureTarget = nullptr;

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

		float4 CenterPos = _Owner->GetTransform()->GetWorldPosition();
		float4 CenterRot = float4::ZERO;

		auto Scene = _Owner->GetOwnerScene().lock();
		if (Scene == nullptr)
		{
			MsgAssert("Scene이 유효하지 않습니다.");
			return;
		}

		// Forward
		Scene->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot, float4(700, 700));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(Scene->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Forword.png"));

		// Back
		Scene->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot + float4(0, 180, 0), float4(512, 512));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(Scene->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Back.png"));

		// Right
		Scene->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot + float4(0, 90, 0), float4(512, 512));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(Scene->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Right.png"));

		// Left
		Scene->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot + float4(0, -90, 0), float4(512, 512));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(Scene->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Left.png"));

		// Top
		Scene->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot + float4(-90, 0, 0), float4(512, 512));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(Scene->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Top.png"));

		// Bottom
		Scene->GetMainCamera()->CaptureCubemap(CenterPos, CenterRot + float4(90, 0, 0), float4(512, 512));
		CaptureTarget->RenderTargetClear();
		CaptureTarget->Merge(Scene->GetMainCamera()->GetCameraRenderTarget());
		Ext_ScreenShoot::RenderTargetShoot_DxTex(CaptureTarget, Path, _CaptureTextureName.data() + std::string("_Bottom.png"));
		CaptureTarget->RenderTargetClear();

		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Forword.png");
		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Back.png");
		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Right.png");
		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Left.png");
		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Top.png");
		Ext_DirectXTexture::LoadTexture(Path + "\\" + _CaptureTextureName.data() + "_Bottom.png");
	}

	CubeTexture = Ext_DirectXTexture::Find(_CaptureTextureName);

	if (nullptr == CubeTexture)
	{
		std::vector<std::shared_ptr<Ext_DirectXTexture>> CubeTextures;
		CubeTextures.reserve(6);

		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_Right.png")));
		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_Left.png")));
		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_Top.png")));
		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_Bottom.png")));
		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_Forword.png")));
		CubeTextures.push_back(Ext_DirectXTexture::Find(_CaptureTextureName.data() + std::string("_Back.png")));

		CubeTexture = Ext_DirectXTexture::LoadCubeMap(_CaptureTextureName, CubeTextures);
	}
}
