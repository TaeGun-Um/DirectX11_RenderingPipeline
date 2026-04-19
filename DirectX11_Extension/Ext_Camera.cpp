#include "PrecompileHeader.h"
#include "Ext_Camera.h"

#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Input.h>

#include "Ext_Scene.h"
#include "Ext_DirectXDevice.h"
#include "Ext_Transform.h"
#include "Ext_MeshComponent.h"
#include "Ext_MeshComponentUnit.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_DirectXMaterial.h"
#include "Ext_DirectXPixelShader.h"
#include "Ext_DirectXBufferSetter.h"

#include "Ext_Blur.h"
#include "Ext_Distortion.h"
#include "Ext_OldFilm.h"
#include "Ext_TextureTest.h"

// 카메라 생성 시 호출
void Ext_Camera::Start()
{
	ViewPortData.TopLeftX = 0;
	ViewPortData.TopLeftY = 0;
	ViewPortData.Width = Base_Windows::GetScreenSize().x;
	ViewPortData.Height = Base_Windows::GetScreenSize().y;
	ViewPortData.MinDepth = 0.0f;
	ViewPortData.MaxDepth = 1.0f;

	Width = ViewPortData.Width;
	Height = ViewPortData.Height;

	// 카메라 최종 렌더타겟
	CameraRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 해당 카메라의 최종 결과물 타겟
	CameraRenderTarget->CreateDepthTexture();
	// CameraRenderTarget->CreateEffect<Ext_Blur>();
	// CameraRenderTarget->CreateEffect<Ext_Distortion>();
	// CameraRenderTarget->CreateEffect<Ext_OldFilm>();
	// CameraRenderTarget->CreateEffect<Ext_TextureTest>();

	// 마스킹 렌더타겟
	// MaskRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 0 Mask

	// 알파 패스 렌더타겟
	AlphaRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 0 AlphaRenderTarget
	AlphaRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 1 PositionTarget (World)

	// 메인패스 렌더타겟 - MeshTarget, PositionTarget, NormalTarget
	MeshRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 0 MeshTarget
	MeshRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 1 PositionTarget (World)
	MeshRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 2 NormalTarget
	MeshRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 3 TangentTarget (PBR만)
	MeshRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 4 BinormalTarget (PBR만)
	MeshRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 5 MaskTarget
	MeshRenderTarget->SetDepthTexture(CameraRenderTarget->GetDepthTexture());

	// 디퍼드 라이트 계산 렌더 타겟(쉐도우 뎁스까지 계산) - DiffuseTarget, SpecularTarget, AmbientTarget, ShadowTarget
	LightRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 0 DiffuseTarget
	LightRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 1 SpecularTarget
	LightRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 2 AmbientTarget
	LightRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 3 ShadowTarget

	// 디퍼드 라이트 Merge 타겟 - Diffuse, Specular, Ambient를 하나로 만듬
	LightMergeRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // Light들 Merge하는 렌더타겟

	// 일반
	// LightRenderTarget(디퍼드 라이트 계산)을 위한 Unit
	LightUnit.MeshComponentUnitInitialize("FullRect", MaterialType::DeferredLight);
	const LightDatas& LTDatas = GetOwnerScene().lock()->GetLightDataBuffer();
	LightUnit.BufferSetter.SetConstantBufferLink("LightDatas", LTDatas);
	LightUnit.BufferSetter.SetTexture(MeshRenderTarget->GetTexture(1), "PositionTex");
	LightUnit.BufferSetter.SetTexture(MeshRenderTarget->GetTexture(2), "NormalTex");
	//LightUnit.BufferSetter.SetTexture("Null.png", TextureType::Shadow);

	// LightMergeRenderTarget(디퍼드 라이트 Merge)를 위한 Unit
	LightMergeUnit.MeshComponentUnitInitialize("FullRect", MaterialType::DeferredMerge);
	// LightMergeUnit.BufferSetter.SetConstantBufferLink("CameraPosition", GetTransform()->GetTransformData()->WorldPosition);
	LightMergeUnit.BufferSetter.SetTexture(MeshRenderTarget->GetTexture(0), "BaseColorTex");
	LightMergeUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(0), "DiffuseTex");
	LightMergeUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(1), "SpecularTex");
	LightMergeUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(2), "AmbientTex");
	LightMergeUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(3), "ShadowTex");
	// LightMergeUnit.BufferSetter.SetTexture(AlphaRenderTarget->GetTexture(0), "AlphaTex");
	//LightMergeUnit.BufferSetter.SetTexture(MeshRenderTarget->GetTexture(1), "BaseColorPosTex");
	//LightMergeUnit.BufferSetter.SetTexture(AlphaRenderTarget->GetTexture(1), "AlphaPosTex");
	LightMergeUnit.SetSampler(SamplerType::PointClamp);

	// 스카이박스용
	SkyBoxRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL);
	SkyBoxTransform = std::make_shared<Ext_Transform>();
	SkyBoxUnit.MeshComponentUnitInitialize("FullBox", "SkyBox");
	std::shared_ptr<Ext_DirectXTexture> Tex = Ext_DirectXTexture::Find("SkyBox");
	SkyBoxUnit.GetBufferSetter().SetTexture(Tex, "CubeMapTex");

}

// 여기는 요소 제거만 진행합니다.
void Ext_Camera::RemoveMeshByActor(std::shared_ptr<Ext_Actor> _DeadActor)
{
	// [1] MeshComponents에서 해당 액터 소유 MeshComponent 제거
	for (auto& [Order, List] : MeshComponents)
	{
		// MeshComp가 유효한 shared_ptr이고 그 소유 Actor가 DeadActor와 동일할 때 뒤로 밀어서 제거
		List.erase(std::remove_if(List.begin(), List.end(),	[&](std::shared_ptr<Ext_MeshComponent>& MeshComp)
				{
					return MeshComp && MeshComp->GetOwnerActor().lock() == _DeadActor;
				}),
			List.end()
		);
	}

	// [2] MeshComponentUnits에서도 제거
	for (auto& [Order, UnitList] : MeshComponentUnits)
	{
		// 각 MeshComponentUnit의 소유 MeshComponent를 얻고 그 MeshComponent가 가리키는 Actor가 DeadActor이면 뒤로 밀어서 제거
		UnitList.remove_if([&](std::shared_ptr<Ext_MeshComponentUnit>& Unit)
			{
				auto MeshComp = Unit->GetOwnerMeshComponent().lock();
				return MeshComp && MeshComp->GetOwnerActor().lock() == _DeadActor;
			});
	}
}

// 현재 카메라 타입에 따라 뷰, 프로젝션, 뷰포트 행렬 세팅
void Ext_Camera::CameraTransformUpdate()
{
	float4 EyePos = GetTransform()->GetWorldPosition();
	float4 EyeDir = GetTransform()->GetLocalForwardVector();
	float4 EyeUp = GetTransform()->GetLocalUpVector();
	ViewMatrix.LookToLH(EyePos, EyeDir, EyeUp);

	switch (CameraType)
	{
	case ProjectionType::Perspective:
	{
		ProjectionMatrix.PerspectiveFovLH(FOV, Width / Height, Near, Far);
		break;
	}
	case ProjectionType::Orthogonal:
	{
		// Projection.OrthographicLH(Width * ZoomRatio, Height * ZoomRatio, Near, Far); 일단 패스
		break;
	}
	case ProjectionType::Unknown:
	{
		MsgAssert("카메라 투영이 설정되지 않았습니다.");
		break;
	}
	}

	ViewPortMatrix.ViewPort(Width, Height, 0.0f, 0.0f);
}

// 뷰포트 세팅 ////////////////////////////////// 테스트용 /////////////////////////
void Ext_Camera::ViewPortSetting()
{
	// 없어도 될듯
	Ext_DirectXDevice::GetContext()->RSSetViewports(1, &ViewPortData);
}

// MeshComponents에 생성된 MeshComponent 넣기
void Ext_Camera::PushMeshComponent(std::shared_ptr<Ext_MeshComponent> _MeshComponent)
{
	if (nullptr == _MeshComponent)
	{
		MsgAssert("MeshComponent가 nullptr 입니다");
		return;
	}

	_MeshComponent->SetOwnerCamera(GetSharedFromThis<Ext_Camera>());
	MeshComponents[GetOrder()].push_back(_MeshComponent);
}

// 생성된 MeshComponentUnit을 카메라의 MeshComponentUnits에 넣기
void Ext_Camera::PushMeshComponentUnit(std::shared_ptr<Ext_MeshComponentUnit> _Unit)
{
	int Order = _Unit->GetOwnerMeshComponent().lock()->GetOrder();
	MeshComponentUnits[Order].push_back(_Unit);
	// 여기서 동적, 정적으로 나눌 수도 있다.
}

// 카메라의 MeshComponents들에 대한 업데이트 및 렌더링 파이프라인 리소스 정렬
void Ext_Camera::Rendering(float _Deltatime)
{
	if (true == bIsSkybox)
	{
		SkyBoxRendering();
	}

	AlphaRenderTarget->RenderTargetClear(); // 디버그 GUI용으로 유지(본 렌더링엔 사용 X)
	MeshRenderTarget->RenderTargetClear(); // 공유 depth(=CameraRT depth) 도 함께 클리어됨

	// 전체 유닛 Z정렬 (far→near). 알파 패스에서 back-to-front 블렌딩을 위해 필요
	std::vector<std::shared_ptr<Ext_MeshComponentUnit>> AllRenderUnits;
	for (auto& [IndexKey, UnitList] : MeshComponentUnits)
	{
		for (auto& Unit : UnitList)
		{
			auto Owner = Unit->GetOwnerMeshComponent().lock();
			if (!Owner || Owner->IsDeath() || !Owner->IsUpdate())
			{
				continue;
			}
			AllRenderUnits.push_back(Unit);
		}
	}

	float4 CamPos = GetTransform()->GetWorldPosition();
	std::sort(AllRenderUnits.begin(), AllRenderUnits.end(),
		[&](const std::shared_ptr<Ext_MeshComponentUnit>& A, const std::shared_ptr<Ext_MeshComponentUnit>& B)
		{
			auto AMesh = A->GetOwnerMeshComponent().lock();
			auto BMesh = B->GetOwnerMeshComponent().lock();
			return (AMesh->GetTransform()->GetWorldPosition() - CamPos).Size()
			 > (BMesh->GetTransform()->GetWorldPosition() - CamPos).Size();
		});

	// 메인 디퍼드 패스 (opaque 전용, 알파는 별도 Forward 패스에서 처리)
	std::unordered_set<std::shared_ptr<Ext_MeshComponent>> UpdatedComponents;
	for (auto& Unit : AllRenderUnits)
	{
		auto Owner = Unit->GetOwnerMeshComponent().lock();
		if (!Owner) continue;
		if (Unit->GetIsAlpha()) continue; // 알파 유닛은 합성 이후 Forward 패스에서 그림

		MeshRenderTarget->RenderTargetSetting();

		if (UpdatedComponents.insert(Owner).second)
		{
			Owner->Rendering(_Deltatime, GetTransform()->GetLocalPosition(), GetViewMatrix(), GetProjectionMatrix());
		}

		Unit->Rendering(_Deltatime);
	}

	// 쉐도우 패스, 뎁스 만들기
	auto& Lights = GetOwnerScene().lock()->GetLights();
	for (auto& [name, CurLight] : Lights)
	{
		std::shared_ptr<Ext_DirectXRenderTarget> CurShadowRenderTarget = CurLight->GetShadowRenderTarget();
		if (!CurShadowRenderTarget) continue; // 세팅 안되어있으면 그릴 필요 없음

		std::shared_ptr<LightData> LTData = CurLight->GetLightData(); // 현재 라이트의 데이터(앞서 업데이트됨)
		CurShadowRenderTarget->RenderTargetSetting(); // 백버퍼에서 지금 렌더 타겟으로 바인딩 변경, 여기다 그리기

		// 쉐도우 뎁스 텍스쳐 만들기
		for (auto& Unit : AllRenderUnits)
		{
			if (!Unit->GetIsShadow()) continue;

			Unit->GetOwnerMeshComponent().lock()->GetTransform()->SetCameraMatrix(GetTransform()->GetLocalPosition(), LTData->LightViewMatrix, LTData->LightProjectionMatrix); // 라이트 기준으로 행렬 세팅
			Unit->RenderUnitShadowSetting(); 

			std::shared_ptr<Ext_DirectXMaterial> ShadowPipeLine;

			if (ShadowType::Static == Unit->GetShadowType())
			{
				ShadowPipeLine = Ext_DirectXMaterial::Find("Shadow");
			}
			else if (ShadowType::Dynamic == Unit->GetShadowType())
			{
				ShadowPipeLine = Ext_DirectXMaterial::Find("DynamicShadow");
			}
			else
			{
				MsgAssert("여기 들어오면 안되는데 뭔가 잘못됨");
			}

			ShadowPipeLine->VertexShaderSetting();
			ShadowPipeLine->RasterizerSetting();
			ShadowPipeLine->PixelShaderSetting();
			ShadowPipeLine->OutputMergerSetting();
			Unit->RenderUnitDraw();
		}
	}

	// 여기 문제있따
	LightRenderTarget->RenderTargetClear();
	LightRenderTarget->RenderTargetSetting();

	GetOwnerScene().lock()->GetLightDataBuffer().LightCount = 0; // 라이트 업데이트 전, 상수버퍼 갯수 초기화(순회하면서 넣어줘야하기 때문)
	for (auto& [name, CurLight] : Lights)
	{
		LightUnit.BufferSetter.SetTexture(CurLight->GetShadowRenderTarget()->GetTexture(0), "ShadowTex");
		LightUnit.Rendering(_Deltatime);
		GetOwnerScene().lock()->GetLightDataBuffer().LightCount++;
	}

	// CameraRenderTarget->Merge(MeshRenderTarget, 0); // Geometry 합치기
	
	// 결과 합치기
	LightMergeRenderTarget->RenderTargetClear();
	LightMergeRenderTarget->RenderTargetSetting();
	LightMergeUnit.Rendering(_Deltatime);

	// === 최종 합성 ===
	// CameraRT의 depth는 opaque 패스에서 공유로 채워진 상태.
	// 컬러만 클리어하고 depth는 유지 → 이후 Forward 알파 패스에서 depth 테스트 가능.
	CameraRenderTarget->RenderTargetColorClear();
	CameraRenderTarget->Merge(SkyBoxRenderTarget);
	CameraRenderTarget->Merge(LightMergeRenderTarget);

	// === Forward 알파 패스 ===
	// 라이팅/스카이 합성 완료된 CameraRT에 직접 그림.
	// StaticAlpha 머티리얼은 AlphaDepth(쓰기 OFF) + BaseBlend(SRC_ALPHA).
	// AllRenderUnits는 far→near 정렬되어 있어 back-to-front 블렌딩 정상.
	CameraRenderTarget->RenderTargetSetting();
	for (auto& Unit : AllRenderUnits)
	{
		auto Owner = Unit->GetOwnerMeshComponent().lock();
		if (!Owner || !Unit->GetIsAlpha()) continue;

		if (UpdatedComponents.insert(Owner).second)
		{
			Owner->Rendering(_Deltatime, GetTransform()->GetLocalPosition(), GetViewMatrix(), GetProjectionMatrix());
		}

		Unit->Rendering(_Deltatime);
	}

	CameraRenderTarget->PostProcessing(GetSharedFromThis<Ext_Camera>(), _Deltatime);
}

// 카메라 조종 (언리얼 에디터 스타일 flycam)
// - 우클릭(FlyCam) 홀드 동안 flycam 활성 + 커서 잠금
// - F1(OnOff) 로 sticky 토글 유지 (레거시 호환)
// - 이동: WASD / Space / Ctrl / Shift 스프린트
// - 마우스 휠: 기본 이동 속도 조절 (상한/하한 clamp)
void Ext_Camera::Update(float _Deltatime)
{
	if (GetSharedFromThis<Ext_Camera>() != GetOwnerScene().lock()->GetMainCamera()) return;

	AccTime += _Deltatime;

	// F1 sticky 토글 (레거시)
	if (Base_Input::IsDown("OnOff") && AccTime >= 0.2f)
	{
		AccTime = 0.f;
		bStickyFlycam = !bStickyFlycam;
	}

	// RMB 홀드 또는 sticky → flycam 활성
	const bool bFlyHeld = Base_Input::IsPress("FlyCam");
	bIsCameraAcc = bStickyFlycam || bFlyHeld;

	// 상태 전이 처리
	if (bIsCameraAcc && !bPrevCameraAcc)
	{
		// flycam 진입: 커서 잠금
		Base_Input::SetCursorLocked(true);
	}
	else if (!bIsCameraAcc && bPrevCameraAcc)
	{
		// flycam 해제: Character가 이어받지 않는 경우(bIsEscape=false)에만 커서 해제
		if (!bIsEscape)
		{
			Base_Input::SetCursorLocked(false);
		}
	}
	bPrevCameraAcc = bIsCameraAcc;

	if (!bIsCameraAcc) return;

	// ---- 마우스 룩 ----
	const float Sensitivity = 0.1f;
	const float DeltaX = static_cast<float>(Base_Input::GetMouseDeltaX());
	const float DeltaY = static_cast<float>(Base_Input::GetMouseDeltaY());

	Yaw   += DeltaX * Sensitivity;
	Pitch += DeltaY * Sensitivity;
	Pitch = std::clamp(Pitch, -89.9f, 89.9f);
	GetTransform()->SetLocalRotation({ Pitch, Yaw, 0.f });

	// ---- 스크롤 휠: 이동 속도 조절 ----
	int WheelDelta = Base_Input::GetMouseWheel();
	if (0 != WheelDelta)
	{
		const float ScaleStep = 1.15f;
		if (WheelDelta > 0) FlyBaseSpeed *= ScaleStep;
		else                 FlyBaseSpeed /= ScaleStep;
		FlyBaseSpeed = std::clamp(FlyBaseSpeed, 20.0f, 5000.0f);
	}

	// ---- 이동 ----
	const float4 Forward = GetTransform()->GetLocalForwardVector();
	const float4 Right   = GetTransform()->GetLocalRightVector();
	const float4 WorldUp = float4::UP; // flycam 수직은 월드 업 기준

	float MoveSpeed = FlyBaseSpeed * _Deltatime;
	if (Base_Input::IsPress("Sprint")) MoveSpeed *= 3.0f;

	float4 Move = { 0.f, 0.f, 0.f, 0.f };
	if (Base_Input::IsPress("Forword")) Move += Forward * MoveSpeed;
	if (Base_Input::IsPress("Back"))    Move -= Forward * MoveSpeed;
	if (Base_Input::IsPress("Right"))   Move += Right   * MoveSpeed;
	if (Base_Input::IsPress("Left"))    Move -= Right   * MoveSpeed;
	if (Base_Input::IsPress("Up"))      Move += WorldUp * MoveSpeed;
	if (Base_Input::IsPress("DOWN"))    Move -= WorldUp * MoveSpeed;

	GetTransform()->AddLocalPosition(Move);
}

void Ext_Camera::CaptureCubemap(const float4& _Pos, const float4& _Rot, const float4& _CaptureScale /*= float4(128, 128)*/)
{
	std::shared_ptr<TransformData> TFData = GetTransform()->GetTransformData();

	GetTransform()->SetLocalPosition(_Pos);
	GetTransform()->SetLocalRotation(_Rot);

	float CurWidth = Width;
	float CurHeight = Height;

	Width = _CaptureScale.x;
	Height = _CaptureScale.y;

	CameraRenderTarget->RenderTargetClear();
	MeshRenderTarget->RenderTargetClear();

	if (nullptr != LightMergeRenderTarget)
	{
		LightMergeRenderTarget->RenderTargetClear();
	}

	CameraTransformUpdate();
	ViewPortSetting();

	// Light
	GetOwnerScene().lock()->GetLightDataBuffer().LightCount = 0;
	for (auto& LightIter : GetOwnerScene().lock()->Lights)
	{
		std::shared_ptr<Ext_Light> CurLight = LightIter.second;

		CurLight->LightUpdate(GetSharedFromThis<Ext_Camera>(), 0.0f);
		GetOwnerScene().lock()->LightDataBuffer.Lights[GetOwnerScene().lock()->LightDataBuffer.LightCount] = *CurLight->GetLightData().get(); // 상수버퍼에 값 넣어주기
		++GetOwnerScene().lock()->LightDataBuffer.LightCount;
	}

	Rendering(0.0f);

	Width = CurWidth;
	Height = CurHeight;

	GetTransform()->SetTransformData(TFData);
}

void Ext_Camera::SkyBoxRendering()
{
	SkyBoxRenderTarget->RenderTargetClear();
	SkyBoxRenderTarget->RenderTargetSetting();

	float4 CamPos = GetTransform()->GetWorldPosition();
	SkyBoxTransform->SetLocalPosition(CamPos);
	SkyBoxTransform->SetCameraMatrix(GetTransform()->GetWorldPosition(), ViewMatrix, ProjectionMatrix);
	SkyBoxUnit.GetBufferSetter().SetConstantBufferLink("TransformData", *SkyBoxTransform->GetTransformData());
	SkyBoxUnit.Rendering(0.0f);
	SkyBoxUnit.GetBufferSetter().AllTextureResourceReset();
}