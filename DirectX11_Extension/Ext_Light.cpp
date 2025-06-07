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
	ShadowRange.x = 1024.0f;
	ShadowRange.y = 1024.0f;
}

void Ext_Light::Start()
{
	std::string SceneName = GetOwnerScene().lock()->GetName();
	std::string Name = GetName();

	GetOwnerScene().lock()->PushLight(GetSharedFromThis<Ext_Light>(), GetName());
	LightType Type = this->GetLightType();

	if ("PointLight1" != Name)
	{
		CreateShadowTarget(Type);
	}
}

bool first = false;

// 라이트 연산 정보 업데이트, 여기서의 Camera는 바라보는 시선, 위치를 의미함
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
		if (1 != LightViewDatas.size())
		{
			LightViewDatas.resize(1);
		}

		std::shared_ptr<class Ext_Transform> LT = GetTransform();
		LightViewDatas[0].LightViewMatrix.LookToLH(LT->GetWorldPosition(), LT->GetLocalForwardVector(), LT->GetLocalUpVector());
		LightViewDatas[0].LightViewInverseMatrix = LightViewDatas[0].LightViewMatrix.InverseReturn();
		LightViewDatas[0].LightProjectionMatrix.OrthographicLH(ShadowRange.x, ShadowRange.y, LTData->NearDistance, LTData->FarDistance/* * 2.0f*/);
		LightViewDatas[0].LightProjectionInverseMatrix = LightViewDatas[0].LightProjectionMatrix.InverseReturn();
		LightViewDatas[0].LightViewProjectionMatrix = LightViewDatas[0].LightViewMatrix * LightViewDatas[0].LightProjectionMatrix;
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

// 라이트 데이터에 모두 전달
void Ext_Light::LightViewSetting(size_t _Index)
{
	if (LightViewDatas.size() <= _Index)
	{
		MsgAssert("Light View Data를 초과해 세팅하려 했습니다");
		_Index = 0;
	}

	LTData->LightViewMatrix = LightViewDatas[_Index].LightViewMatrix;
	LTData->LightViewInverseMatrix = LightViewDatas[_Index].LightViewInverseMatrix;
	LTData->LightProjectionMatrix = LightViewDatas[_Index].LightProjectionMatrix;
	LTData->LightProjectionInverseMatrix = LightViewDatas[_Index].LightProjectionInverseMatrix;
	LTData->LightViewProjectionMatrix = LightViewDatas[_Index].LightViewProjectionMatrix;
}

void Ext_Light::CreateShadowTarget(LightType _Type)
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
		ShadowRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT_R16_FLOAT, { LTData->ShadowTargetSizeX, LTData->ShadowTargetSizeY }, float4::ZERO);
		int a = 0;
	}
	break;
	}
}
