#include "PrecompileHeader.h"
#include "Ext_Light.h"
#include "Ext_Camera.h"
#include "Ext_Transform.h"

Ext_Light::Ext_Light()
{
	LTData = std::make_shared<LightData>();
}

void Ext_Light::SetLightRange(float _Range)
{
	LTData->LightNear = 1.0f;
	LTData->LightFar = _Range;
}

bool first = false;

void Ext_Light::LightUpdate(std::shared_ptr<Ext_Camera> _Camera, float _DeltaTime)
{
	LTData->LightWorldPosition = GetTransform()->GetWorldPosition();
	LTData->LightForward = GetTransform()->GetLocalForwardVector();
	LTData->LightViewSpacePosition = LTData->LightWorldPosition * _Camera->GetViewMatrix();
	LTData->LightViewSpaceForward = LTData->LightForward * _Camera->GetViewMatrix();
	LTData->LightViewSpaceBack = -LTData->LightForward;
	// LightDatas.CameraPosition = _Camera->GetTransform()->GetWorldPosition() * _Camera->GetViewMatrix();
	// LightDatas.CameraWorldPosition = _Camera->GetTransform()->GetWorldPosition();

	if (LTData->LightType == static_cast<int>(LightType::Directional))
	{
		// LightDatas.LightViewMatrix.LookToLH(GetTransform()->GetWorldPosition(), GetTransform()->GetLocalForwardVector(),	GetTransform()->GetLocalUpVector());
		// LightDatas.LightViewInverseMatrix = LightDatas.LightViewMatrix.InverseReturn();
		// LightDatas.LightProjectionMatrix.OrthographicLH( ShadowRange.x, ShadowRange.y, LightDatas.LightNear, LightDatas.LightFar * 2.0f);
		// LightDatas.LightProjectionInverseMatrix = LightDatas.LightProjectionMatrix.InverseReturn();
		// LightDatas.LightViewProjectionMatrix = LightDatas.LightViewMatrix * LightDatas.LightProjectionMatrix;
	}
	//else if (LightDataValue.LightType == static_cast<int>(LightType::Point))
	//{
	//	if (6 != ViewDatas.size())
	//	{
	//		ViewDatas.resize(6);
	//	}

	//	for (size_t i = 0; i < 6; i++)
	//	{
	//		float4 WorldPos = GetTransform()->GetWorldPosition();
	//		float4 EyeDir = PointViewDatas[i][0];
	//		float4 UpDir = PointViewDatas[i][1];

	//		ViewDatas[i].LightViewMatrix.LookToLH(
	//			WorldPos,
	//			EyeDir,
	//			UpDir);

	//		ViewDatas[i].LightViewInverseMatrix = ViewDatas[i].LightViewMatrix.InverseReturn();

	//		ViewDatas[i].LightProjectionMatrix.PerspectiveFovLH(
	//			90.0f,
	//			1.0f,
	//			LightDataValue.LightNear,
	//			LightDataValue.LightFar * 2.0f);

	//		ViewDatas[i].LightProjectionInverseMatrix = ViewDatas[i].LightProjectionMatrix.InverseReturn();
	//		ViewDatas[i].LightViewProjectionMatrix = ViewDatas[i].LightViewMatrix * ViewDatas[i].LightProjectionMatrix;
	//	}
	//}
	//else // Spot
	//{
	//	if (1 != ViewDatas.size())
	//	{
	//		ViewDatas.resize(1);
	//	}

	//	float LightAngle = LightDataValue.LightAngle;

	//	if (1 > LightAngle)
	//	{
	//		LightAngle = 1;
	//	}

	//	if (1 == ViewDatas.size())
	//	{
	//		ViewDatas.resize(1);
	//	}

	//	ViewDatas[0].LightViewMatrix.LookToLH(
	//		GetTransform()->GetWorldPosition(),
	//		GetTransform()->GetWorldForwardVector(),
	//		GetTransform()->GetWorldUpVector());

	//	ViewDatas[0].LightViewInverseMatrix = ViewDatas[0].LightViewMatrix.InverseReturn();

	//	ViewDatas[0].LightProjectionMatrix.PerspectiveFovLH(
	//		LightAngle,
	//		1.0f,
	//		LightDataValue.LightNear,
	//		LightDataValue.LightFar);

	//	ViewDatas[0].LightProjectionInverseMatrix = ViewDatas[0].LightProjectionMatrix.InverseReturn();

	//	ViewDatas[0].LightViewProjectionMatrix = ViewDatas[0].LightViewMatrix * ViewDatas[0].LightProjectionMatrix;
	//}
}