#include "PrecompileHeader.h"
#include "Ext_Camera.h"
#include <DirectX11_Base/Base_Windows.h>
#include "Ext_Transform.h"

Ext_Camera::Ext_Camera()
{
}

Ext_Camera::~Ext_Camera()
{
}

float4x4 Ext_Camera::GetViewMatrix()
{
	return float4x4();
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
	View.LookToLH(EyePos, EyeDir, EyeUp);

	switch (ProjectionType)
	{
	case CameraType::None:
	{
		MsgAssert("ī�޶� ������ �������� �ʾҽ��ϴ�.");
		break;
	}
	case CameraType::Perspective:
		Projection.PerspectiveFovLH(FOV, Width / Height, Near, Far);
		break;
	case CameraType::Orthogonal:
		// Projection.OrthographicLH(Width * ZoomRatio, Height * ZoomRatio, Near, Far);
		// ������ �ϴ� �н�
		break;
	default:
		break;
	}

	ViewPort.ViewPort(Width, Height, 0.0f, 0.0f);
}
