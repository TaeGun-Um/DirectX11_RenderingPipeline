#include "PrecompileHeader.h"
#include "Ext_Camera.h"

#include <DirectX11_Base/Base_Windows.h>

#include "Ext_DirectXDevice.h"
#include "Ext_Transform.h"
#include "Ext_MeshComponent.h"
#include "Ext_MeshComponentUnit.h"

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
			if (!CurMeshComponent->GetIsUpdate()) continue;
			else
			{
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