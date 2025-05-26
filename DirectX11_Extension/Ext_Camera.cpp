#include "PrecompileHeader.h"
#include "Ext_Camera.h"

#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Input.h>

#include "Ext_Scene.h"
#include "Ext_DirectXDevice.h"
#include "Ext_Transform.h"
#include "Ext_MeshComponent.h"
#include "Ext_MeshComponentUnit.h"

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
	// RenderPath Path = _Unit->GetMaterial()->GetPixelShader()->GetRenderPath();
	MeshComponentUnits[_Path][Order].push_back(_Unit);

	// 여기서 동적, 정적으로 나눌 수도 있다.
}

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
}

// 카메라의 MeshComponents들에 대한 업데이트 및 렌더링 파이프라인 리소스 정렬
void Ext_Camera::Rendering(float _Deltatime)
{
	// MeshComponents 렌더링 업데이트 시작
	for (auto& [Key, MeshComponentList] : MeshComponents)
	{
		// [!] 필요하면 ZSort 실시(나중에)

		for (const std::shared_ptr<Ext_MeshComponent>& CurMeshComponent : MeshComponentList)
		{
			if (!CurMeshComponent->IsUpdate() || CurMeshComponent->IsDeath()) continue;
			else
			{
				std::string Name = CurMeshComponent->GetName();
				CurMeshComponent->Rendering(_Deltatime, GetViewMatrix(), GetProjectionMatrix()); // [3] 현재 MeshComponent에게 카메라의 View, Projection 곱해주기
				// [!] 필요하면 픽셀 셰이더에서 활용할 Value들 업데이트
			}
		}

	}

	// 컴포넌트 메시의 유닛들 순회
	for (auto& [RenderPathKey, UnitMap] : MeshComponentUnits)
	{
		//switch (RenderPathKey)
		//{
		//case RenderPath::Forward:
		//{

		//	break;
		//}
		//case RenderPath::Deferred:
		//{

		//	break;
		//}
		//case RenderPath::Alpha:
		//{

		//	break;
		//}
		//case RenderPath::Unknown:
		//{
		//	
		//	break;
		//}
		//default:
		//{
		//	MsgAssert("what?");
		//	break;
		//}
		//}

		for (auto& [IndexKey, UnitList] : UnitMap)
		{
			for (auto& Unit : UnitList)
			{
				auto OwnerMeshComponent = Unit->GetOwnerMeshComponent().lock();
				if (!OwnerMeshComponent || OwnerMeshComponent->IsDeath() || !OwnerMeshComponent->IsUpdate())
				{
					continue; // 죽은 MeshComponent의 유닛은 렌더링하지 않음
				}
				Unit->Rendering(_Deltatime); // 예시
			}
		}
	}
		
		// <<추가 필요>>
		// 렌더링 파이프라인의 각 단계 호출, << Ext_Material에서 진행 >>
		// 1. Resterizer : RasterizerPtr->SetFILL_MODE(FILL_MODE);, GameEngineDevice::GetContext()->RSSetState(CurState);
		// 2. OutputMerget : GameEngineDevice::GetContext()->OMSetBlendState(State, nullptr, Mask);, 
		// 3. GameEngineDevice::GetContext()->OMSetDepthStencilState(State, 0);
}










// Zsort 로직(저장)
///////////////////////////////////////////////////////////////////////////////////////////////////////////int Order = RenderGroupStartIter->first;
///////////////////////////////////////////////////////////////////////////////////////////////////////////std::map<int, SortType>::iterator SortIter = SortValues.find(Order);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////if (SortIter != SortValues.end() && SortIter->second != SortType::None)
///////////////////////////////////////////////////////////////////////////////////////////////////////////{
///////////////////////////////////////////////////////////////////////////////////////////////////////////	if (SortIter->second == SortType::ZSort)
///////////////////////////////////////////////////////////////////////////////////////////////////////////	{
///////////////////////////////////////////////////////////////////////////////////////////////////////////		for (std::shared_ptr<GameEngineRenderer>& Render : RenderGroup)
///////////////////////////////////////////////////////////////////////////////////////////////////////////		{
///////////////////////////////////////////////////////////////////////////////////////////////////////////			Render->CalSortZ(this);
///////////////////////////////////////////////////////////////////////////////////////////////////////////		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////		RenderGroup.sort([](std::shared_ptr<GameEngineRenderer>& _Left, std::shared_ptr<GameEngineRenderer>& _Right)
///////////////////////////////////////////////////////////////////////////////////////////////////////////			{
///////////////////////////////////////////////////////////////////////////////////////////////////////////				return _Left->CalZ > _Right->CalZ;
///////////////////////////////////////////////////////////////////////////////////////////////////////////			});
///////////////////////////////////////////////////////////////////////////////////////////////////////////	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////}

// 카메라 조종
void Ext_Camera::Update(float _Deltatime)
{
	if (GetSharedFromThis<Ext_Camera>() != GetOwnerScene().lock()->GetMainCamera()) return;

	AccTime += _Deltatime;

	if (Base_Input::IsDown("OnOff") && AccTime >= 1.0f)
	{
		AccTime = 0.f;
		bIsCameraSwitch();
	}

	if (!bIsCameraAcc) return;

	static POINT PrevMouse = { Base_Windows::GetScreenSize().x / 2, Base_Windows::GetScreenSize().y / 2 };
	POINT CurMouse;
	GetCursorPos(&CurMouse); // 현재 마우스 위치를 화면 기준 좌표계로 얻어옴, 이값과 PrevMouse 차이를 통해 마우스가 얼마나 움직였나 파악 가능

	int DeltaX = CurMouse.x - PrevMouse.x;
	int DeltaY = CurMouse.y - PrevMouse.y;

	float Sensitivity = 0.1f; // 움직임 대비 회전량 강도(클수록 더 크게 회전)
	Yaw += DeltaX * Sensitivity;     // 좌우 (Y축 회전)
	Pitch += DeltaY * Sensitivity;   // 상하 (X축 회전, Y 반전 적용됨)

	Pitch = std::clamp(Pitch, -89.9f, 89.9f); // 상하 극단적 회전 제한

	GetTransform()->SetLocalRotation({ Pitch, Yaw, 0.f });

	// 커서 중앙 고정
	SetCursorPos(PrevMouse.x, PrevMouse.y);

	// 회전 이후 현재 방향 벡터 얻기
	float4 Forward = GetTransform()->GetLocalForwardVector(); // Z축
	float4 Right = GetTransform()->GetLocalRightVector();     // X축
	float4 Up = GetTransform()->GetLocalUpVector();           // Y축

	float MoveSpeed = 10.f * _Deltatime;
	float4 MoveDelta = { 0.f, 0.f, 0.f };
	if (Base_Input::IsPress("Forword"))
	{
		MoveDelta += Forward * MoveSpeed;
	}
	if (Base_Input::IsPress("Back"))
	{
		MoveDelta -= Forward * MoveSpeed;
	}
	if (Base_Input::IsPress("Right"))
	{
		MoveDelta += Right * MoveSpeed;
	}
	if (Base_Input::IsPress("Left"))
	{
		MoveDelta -= Right * MoveSpeed;
	}
	if (Base_Input::IsPress("Up"))
	{
		MoveDelta += Up * MoveSpeed;
	}
	if (Base_Input::IsPress("Down"))
	{
		MoveDelta -= Up * MoveSpeed;
	}

	GetTransform()->AddLocalPosition(MoveDelta);
}