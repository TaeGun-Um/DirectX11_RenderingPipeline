#include "PrecompileHeader.h"
#include "Ext_Camera.h"

#include <DirectX11_Base/Base_Windows.h>

#include "Ext_Transform.h"
#include "Ext_MeshComponent.h"

Ext_Camera::Ext_Camera()
{
}

Ext_Camera::~Ext_Camera()
{
}

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

void Ext_Camera::CameraTransformUpdate()
{
	float4 EyeDir = GetTransform()->GetLocalForwardVector();
	float4 EyeUp = GetTransform()->GetLocalUpVector();
	float4 EyePos = GetTransform()->GetWorldPosition();
	ViewMatrix.LookToLH(EyePos, EyeDir, EyeUp);

	switch (ProjectionType)
	{
	case CameraType::None:
	{
		MsgAssert("카메라 투영이 설정되지 않았습니다.");
		break;
	}
	case CameraType::Perspective:
		ProjectionMatrix.PerspectiveFovLH(FOV, Width / Height, Near, Far);
		break;
	case CameraType::Orthogonal:
		// Projection.OrthographicLH(Width * ZoomRatio, Height * ZoomRatio, Near, Far);
		// 직교는 일단 패스
		break;
	default:
		break;
	}

	ViewPortMatrix.ViewPort(Width, Height, 0.0f, 0.0f);
}

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