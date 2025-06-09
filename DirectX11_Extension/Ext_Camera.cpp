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

	MeshRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 0 MeshTarget
	MeshRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 1 PositionTarget (World)
	MeshRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 2 PositionTarget (WorldView)
	MeshRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 3 NormalTarget (World)
	MeshRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 4 NormalTarget (WorldView)
	MeshRenderTarget->CreateDepthTexture();

	LightRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 0 DiffuseTarget
	LightRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 1 SpecularTarget
	LightRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 2 AmbientTarget
	LightRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 3 ShadowTarget

	LightPostRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 0 DiffuseTarget
	LightPostRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 1 SpecularTarget
	LightPostRenderTarget->AddNewTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 2 AmbientTarget

	LightMergeRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // Light들 Merge하는 렌더타겟
	CameraRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Base_Windows::GetScreenSize(), float4::ZERONULL); // 해당 카메라의 최종 결과물 타겟

	// LightRenderTarget
	LightUnit.MeshComponentUnitInitialize("FullRect", MaterialType::DeferredLight);
	const LightDatas& LTDatas = GetOwnerScene().lock()->GetLightDataBuffer();
	LightUnit.BufferSetter.SetConstantBufferLink("LightDatas", LTDatas);
	LightUnit.BufferSetter.SetTexture(MeshRenderTarget->GetTexture(1), "PositionTex");
	LightUnit.BufferSetter.SetTexture(MeshRenderTarget->GetTexture(3), "NormalTex");
	//LightUnit.SetSampler(SamplerType::PointClamp);

	// // LightPostRenderTarget
	// LightPostUnit.MeshComponentUnitInitialize("FullRect", MaterialType::DeferredPost);
	// LightPostUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(0), "DiffuseTex");
	// LightPostUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(1), "SpecularTex");
	// LightPostUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(2), "AmbientTex");
	// LightPostUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(3), "ShadowTex");
	// //LightUnit.SetSampler(SamplerType::PointClamp);
	// 
	// // LightMergeRenderTarget
	// LightMergeUnit.MeshComponentUnitInitialize("FullRect", MaterialType::DeferredMerge);
	// LightMergeUnit.BufferSetter.SetTexture(MeshRenderTarget->GetTexture(0), "BaseColorTex");
	// LightMergeUnit.BufferSetter.SetTexture(LightPostRenderTarget->GetTexture(0), "DiffuseTex");
	// LightMergeUnit.BufferSetter.SetTexture(LightPostRenderTarget->GetTexture(1), "SpecularTex");
	// LightMergeUnit.BufferSetter.SetTexture(LightPostRenderTarget->GetTexture(2), "AmbientTex");
	// //LightUnit.SetSampler(SamplerType::PointClamp);

		// LightMergeRenderTarget
	LightMergeUnit.MeshComponentUnitInitialize("FullRect", MaterialType::DeferredMerge);
	LightMergeUnit.BufferSetter.SetTexture(MeshRenderTarget->GetTexture(0), "BaseColorTex");
	LightMergeUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(0), "DiffuseTex");
	LightMergeUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(1), "SpecularTex");
	LightMergeUnit.BufferSetter.SetTexture(LightRenderTarget->GetTexture(2), "AmbientTex");
	//LightUnit.SetSampler(SamplerType::PointClamp);
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
	for (auto& [Path, MapByOrder] : MeshComponentUnits)
	{
		for (auto& [Order, UnitList] : MapByOrder)
		{
			// 각 MeshComponentUnit의 소유 MeshComponent를 얻고 그 MeshComponent가 가리키는 Actor가 DeadActor이면 뒤로 밀어서 제거
			UnitList.remove_if([&](std::shared_ptr<Ext_MeshComponentUnit>& Unit)
				{
					auto MeshComp = Unit->GetOwnerMeshComponent().lock();
					return MeshComp && MeshComp->GetOwnerActor().lock() == _DeadActor;
				});
		}
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
void Ext_Camera::PushMeshComponentUnit(std::shared_ptr<Ext_MeshComponentUnit> _Unit, RenderPath _Path /*= RenderPath::None*/)
{
	int Order = _Unit->GetOwnerMeshComponent().lock()->GetOrder();
	RenderPath Path; /*= _Unit->GetMaterial()->GetPixelShader()->GetRenderPath();*/
	Path = RenderPath::Forward;

	MeshComponentUnits[_Path][Order].push_back(_Unit);

	// 여기서 동적, 정적으로 나눌 수도 있다.
}

// 카메라의 MeshComponents들에 대한 업데이트 및 렌더링 파이프라인 리소스 정렬
void Ext_Camera::Rendering(float _Deltatime)
{
	MeshRenderTarget->RenderTargetClear();
	MeshRenderTarget->RenderTargetSetting();

	// 전체 유닛 Z정렬 후 렌더링
	std::vector<std::shared_ptr<Ext_MeshComponentUnit>> AllRenderUnits;
	for (auto& [RenderPathKey, UnitMap] : MeshComponentUnits)
	{
		for (auto& [IndexKey, UnitList] : UnitMap)
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

	// 메인 렌더링 패스, 백버퍼가 세팅되어 있어서 거기에 그림
	std::unordered_set<std::shared_ptr<Ext_MeshComponent>> UpdatedComponents;
	for (auto& Unit : AllRenderUnits)
	{
		auto Owner = Unit->GetOwnerMeshComponent().lock();
		if (!Owner) continue;

		// View/Projection은 한 번만 업데이트
		if (UpdatedComponents.insert(Owner).second)
		{
			Owner->Rendering(_Deltatime, GetViewMatrix(), GetProjectionMatrix()); // 행렬 업데이트
		}

		Unit->Rendering(_Deltatime); // 렌더링 파이프라인 세팅 후 드로우콜
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

			Unit->GetOwnerMeshComponent().lock()->GetTransform()->SetCameraMatrix(LTData->LightViewMatrix, LTData->LightProjectionMatrix); // 라이트 기준으로 행렬 세팅
			Unit->RenderUnitShadowSetting(); 
			auto PShadow = Ext_DirectXMaterial::Find("Shadow");
			PShadow->VertexShaderSetting();
			PShadow->RasterizerSetting();
			PShadow->PixelShaderSetting();
			PShadow->OutputMergerSetting();
			Unit->RenderUnitDraw();
		}
	}

	LightRenderTarget->RenderTargetClear();
	LightRenderTarget->RenderTargetSetting();

	GetOwnerScene().lock()->GetLightDataBuffer().LightCount = 0; // 라이트 업데이트 전, 상수버퍼 갯수 초기화(순회하면서 넣어줘야하기 때문)
	for (auto& [name, CurLight] : Lights)
	{
		LightUnit.BufferSetter.SetTexture(CurLight->GetShadowRenderTarget()->GetTexture(0), "ShadowTex");
		LightUnit.Rendering(_Deltatime);
		GetOwnerScene().lock()->GetLightDataBuffer().LightCount++;
	}

	//LightPostRenderTarget->RenderTargetClear();
	//LightPostRenderTarget->RenderTargetSetting();
	//LightMergeUnit.Rendering(_Deltatime);

	LightMergeRenderTarget->RenderTargetClear();
	LightMergeRenderTarget->RenderTargetSetting();
	LightMergeUnit.Rendering(_Deltatime);

	CameraRenderTarget->RenderTargetClear();
	CameraRenderTarget->Merge(MeshRenderTarget, 0);
	CameraRenderTarget->Merge(LightMergeRenderTarget);
	CameraRenderTarget->Merge(LightRenderTarget, 3);
}

// 카메라 조종
void Ext_Camera::Update(float _Deltatime)
{
	if (GetSharedFromThis<Ext_Camera>() != GetOwnerScene().lock()->GetMainCamera()) return;

	AccTime += _Deltatime;

	// (A) 카메라 모드(추적/자유) 전환 체크
	if (Base_Input::IsDown("OnOff") && AccTime >= 0.2f)
	{
		AccTime = 0.f;
		bIsCameraSwitch();
	}

	// 마우스 포커스 토글 (F4 키를 눌렀을 때)
	// “ToggleMouse”라는 입력을 F4 키에 매핑했다고 가정
	if (Base_Input::IsDown("Escape"))
	{
		// 마우스 캡처 상태를 반전시키고, 
		// 캡처 중일 때는 커서를 숨기고 중앙 고정, 
		// 캡처 해제 시에는 커서를 보이도록
		bIsEscape = !bIsEscape;
		if (bIsEscape)
		{
			// 캡처 상태: 화면 중앙으로 커서를 가져오고, 커서 숨기기
			POINT center = {
				static_cast<long>(Base_Windows::GetScreenSize().x / 2),
				static_cast<long>(Base_Windows::GetScreenSize().y / 2)
			};
			SetCursorPos(center.x, center.y);
			ShowCursor(FALSE);
		}
		else
		{
			// 캡처 해제: 커서 보이기 (이후 사용자가 자유롭게 움직일 수 있음)
			ShowCursor(TRUE);
		}
	}

	if (!bIsCameraAcc)
		return;

	if (!bPrevCameraAcc && bIsCameraAcc)
	{
		// (A) 캐릭터 추적 모드 → 자유 모드 진입
		GetTransform()->SetLocalPosition(SavedPos);
		GetTransform()->SetLocalRotation(SavedRot);

		// 자유 모드로 바뀔 때, 마우스 캡처를 자동으로 활성화(원한다면)
		// bIsEscape = true;
		// ShowCursor(FALSE);
	}
	else if (bPrevCameraAcc && !bIsCameraAcc)
	{
		// (B) 자유 모드 → 캐릭터 추적 모드로 복귀
		SavedPos = GetTransform()->GetWorldPosition();
		SavedRot = GetTransform()->GetLocalRotation();
		// 이때 마우스 캡처 해제 (원한다면)
		// bIsEscape = false;
		// ShowCursor(TRUE);
	}

	bPrevCameraAcc = bIsCameraAcc;

	// ★ “자유 모드이면서” “마우스 캡처 모드일 때만” 마우스Δ 계산 및 카메라 이동/회전 실행
	if (bIsCameraAcc && bIsEscape)
	{
		// 커서 기준점(화면 중앙) 미리 계산해 둠
		static POINT PrevMouse = {
			static_cast<long>(Base_Windows::GetScreenSize().x / 2),
			static_cast<long>(Base_Windows::GetScreenSize().y / 2)
		};

		POINT CurMouse;
		GetCursorPos(&CurMouse);

		int DeltaX = CurMouse.x - PrevMouse.x;
		int DeltaY = CurMouse.y - PrevMouse.y;

		float Sensitivity = 0.1f;
		Yaw += DeltaX * Sensitivity;
		Pitch += DeltaY * Sensitivity;
		Pitch = std::clamp(Pitch, -89.9f, 89.9f);

		GetTransform()->SetLocalRotation({ Pitch, Yaw, 0.f });

		// 마우스 Δ 처리가 끝났으면 다시 중앙으로 고정
		SetCursorPos(PrevMouse.x, PrevMouse.y);

		// 카메라 이동
		float4 Forward = GetTransform()->GetLocalForwardVector();
		float4 Right = GetTransform()->GetLocalRightVector();
		float4 Up = GetTransform()->GetLocalUpVector();

		float MoveSpeed = 200.f * _Deltatime;
		float4 MoveDelta = { 0.f, 0.f, 0.f };

		if (Base_Input::IsPress("Forword"))  MoveDelta += Forward * MoveSpeed;
		if (Base_Input::IsPress("Back"))     MoveDelta -= Forward * MoveSpeed;
		if (Base_Input::IsPress("Right"))    MoveDelta += Right * MoveSpeed;
		if (Base_Input::IsPress("Left"))     MoveDelta -= Right * MoveSpeed;
		if (Base_Input::IsPress("Up"))       MoveDelta += Up * MoveSpeed;
		if (Base_Input::IsPress("Down"))     MoveDelta -= Up * MoveSpeed;

		GetTransform()->AddLocalPosition(MoveDelta);
	}
}