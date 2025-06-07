#include "PrecompileHeader.h"
#include "Ext_Light.h"
#include "Ext_Scene.h"
#include "Ext_Camera.h"
#include "Ext_Transform.h"
#include "Ext_DirectXRenderTarget.h"

Ext_Light::Ext_Light()
{
	LTData = std::make_shared<LightData>();
	LTData->bIsLightSet = true;
	LTData->ShadowTargetSizeX = 1024;
	LTData->ShadowTargetSizeY = 1024;
}

void Ext_Light::Start()
{
	GetOwnerScene().lock()->PushLight(GetSharedFromThis<Ext_Light>(), GetName());
	LightType Type = this->GetLightType();
	CreateShadowTarget(ShadowRenderTarget, Type);
}

bool first = false;

// 여기서의 Camera는 바라보는 시선, 위치를 의미함
void Ext_Light::LightUpdate(std::shared_ptr<Ext_Camera> _Camera, float _DeltaTime)
{
	if (!first && "DirectionalLight" == this->GetName())
	{
		first = true;
		GetTransform()->AddLocalRotation({ 45.0f, 0.f, 0.f});
		GetTransform()->AddLocalRotation({ 0.f, 45.f, 0.f });
	}

	LTData->LightWorldPosition = GetTransform()->GetWorldPosition();
	//LTData->LightViewPosition = LTData->LightWorldPosition * _Camera->GetViewMatrix();
	LTData->LightForwardVector = GetTransform()->GetLocalForwardVector();
	//LTData->LightBackwordVector = -LTData->LightForwardVector;
	//LTData->LightViewForwardVector = LTData->LightForwardVector * _Camera->GetViewMatrix();

	LTData->CameraWorldPosition = _Camera->GetTransform()->GetWorldPosition();
	//LTData->CameraViewPosition = _Camera->GetTransform()->GetWorldPosition() * _Camera->GetViewMatrix();
	//LTData->CameraForwardVector = _Camera->GetTransform()->GetLocalForwardVector();
	//LTData->CameraViewForwardVector = LTData->CameraForwardVector * _Camera->GetViewMatrix();

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

void Ext_Light::CreateShadowTarget(std::shared_ptr<Ext_DirectXRenderTarget> _Target, LightType _Type)
{
	switch (_Type)
	{
	case LightType::Point:
	{
		//_Target->AddNewCubeTexture(DXGI_FORMAT_R16_FLOAT, { LightDataValue.ShadowTargetSizeX, LightDataValue.ShadowTargetSizeY }, float4(100000, 0, 0, 1.0f));
	}
	break;

	case LightType::Spot:
	{
		//_Target->AddNewTexture(DXGI_FORMAT_R16_FLOAT, { LightDataValue.ShadowTargetSizeX, LightDataValue.ShadowTargetSizeY }, float4(100000, 0, 0, 1.0f));
	}
	break;

	default:
	{
		_Target = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT_R16_FLOAT, { LTData->ShadowTargetSizeX, LTData->ShadowTargetSizeY }, float4::RED);
		int a = 0;
	}
	break;
	}
}
