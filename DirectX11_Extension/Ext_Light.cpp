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
	GetOwnerScene().lock()->PushLight(GetSharedFromThis<Ext_Light>(), GetName());
	ShadowRenderTarget = Ext_DirectXRenderTarget::CreateRenderTarget(DXGI_FORMAT_R32_FLOAT, { LTData->ShadowTargetSizeX, LTData->ShadowTargetSizeY }, float4::RED);
	ShadowRenderTarget->CreateDepthTexture();
}

// 라이트 연산 정보 업데이트, 여기서의 Camera는 바라보는 시선, 위치를 의미함
void Ext_Light::LightUpdate(std::shared_ptr<Ext_Camera> _Camera, float _DeltaTime)
{
	LTData->CameraViewInverseMatrix = _Camera->GetViewMatrix().InverseReturn();

	LTData->LightWorldPosition = GetTransform()->GetWorldPosition();
	//LTData->LightViewPosition = LTData->LightWorldPosition * _Camera->GetViewMatrix();
	LTData->LightForwardVector = GetTransform()->GetLocalForwardVector();
	//LTData->LightBackwordVector = -LTData->LightForwardVector;
	//LTData->LightViewForwardVector = LTData->LightForwardVector * _Camera->GetViewMatrix();

	LTData->CameraWorldPosition = _Camera->GetTransform()->GetWorldPosition();
	//LTData->CameraViewPosition = _Camera->GetTransform()->GetWorldPosition() * _Camera->GetViewMatrix();
	//LTData->CameraForwardVector = _Camera->GetTransform()->GetLocalForwardVector();
	//LTData->CameraViewForwardVector = LTData->CameraForwardVector * _Camera->GetViewMatrix();

	std::shared_ptr<class Ext_Transform> LT = GetTransform();
	LTData->LightViewMatrix.LookToLH(LT->GetWorldPosition(), LT->GetLocalForwardVector(), LT->GetLocalUpVector());
	LTData->LightViewInverseMatrix = LTData->LightViewMatrix.InverseReturn();
	LTData->LightProjectionMatrix.OrthographicLH(ShadowRange.x, ShadowRange.y, LTData->NearDistance, LTData->FarDistance/* * 2.0f*/);
	LTData->LightProjectionInverseMatrix = LTData->LightProjectionMatrix.InverseReturn();
	LTData->LightViewProjectionMatrix = LTData->LightViewMatrix * LTData->LightProjectionMatrix;
}
