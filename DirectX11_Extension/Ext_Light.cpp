#include "PrecompileHeader.h"
#include "Ext_Light.h"
#include "Ext_Scene.h"
#include "Ext_Camera.h"
#include "Ext_Transform.h"
#include "Ext_DirectXRenderTarget.h"

#include <array>

Ext_Light::Ext_Light()
{
	LTData = std::make_shared<LightData>();
	LTData->bIsLightSet = true;
	LTData->ShadowTargetSizeX = 4096;
	LTData->ShadowTargetSizeY = 4096;
	ShadowRange.x = 2048.0f;
	ShadowRange.y = 2048.0f;
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
	LTData->CameraWorldPosition = _Camera->GetTransform()->GetWorldPosition(); // 카메라 월드 위치
	LTData->CameraViewInverseMatrix = _Camera->GetViewMatrix().InverseReturn(); // 카메라 뷰 역행렬

	LTData->LightWorldPosition = GetTransform()->GetWorldPosition(); // 빛 월드 위치
	LTData->LightForwardVector = GetTransform()->GetLocalForwardVector(); // 빛 방향 벡터

	LTData->LightViewMatrix.LookToLH(GetTransform()->GetWorldPosition(), GetTransform()->GetLocalForwardVector(), GetTransform()->GetLocalUpVector()); // 빛 기준 뷰 행렬 생성

    if (LTData->LightType == static_cast<int>(LightType::Directional))
    {
        //ViewPortUpdate(_Camera);
    }

	LTData->LightProjectionMatrix.OrthographicLH(ShadowRange.x, ShadowRange.y, LTData->NearDistance, LTData->FarDistance * 2.0f); // 빛의 직교 프로젝션 행렬 생성
	LTData->LightViewProjectionMatrix = LTData->LightViewMatrix * LTData->LightProjectionMatrix; // 빛 뷰프로젝션 행렬 생성
	LTData->LightViewInverseMatrix = LTData->LightViewMatrix.InverseReturn(); // 빛 뷰 역행렬
	LTData->LightProjectionInverseMatrix = LTData->LightProjectionMatrix.InverseReturn();
}

void Ext_Light::ViewPortUpdate(std::shared_ptr<Ext_Camera> _Camera)
{
    // 2) 카메라 절두체 8개 NDC 코너 정의
    std::array<float4, 8> ndc;
    ndc[0] = float4(-1, -1, 0, 1);
    ndc[1] = float4(+1, -1, 0, 1);
    ndc[2] = float4(+1, +1, 0, 1);
    ndc[3] = float4(-1, +1, 0, 1);
    ndc[4] = float4(-1, -1, 1, 1);
    ndc[5] = float4(+1, -1, 1, 1);
    ndc[6] = float4(+1, +1, 1, 1);
    ndc[7] = float4(-1, +1, 1, 1);

    // 3) 카메라 View·Proj 역행렬
    float4x4 camVP = _Camera->GetViewMatrix() * _Camera->GetProjectionMatrix();
    float4x4 invCamVP = camVP.InverseReturn();

    // 4) NDC → 월드
    std::vector<float4> worldCorners(8);
    for (int i = 0; i < 8; ++i)
    {
        float4 v = ndc[i] * invCamVP;
        worldCorners[i] = v / v.w;
    }

    // 5) 월드 → 라이트 공간
    std::vector<float4> lightCorners(8);
    for (int i = 0; i < 8; ++i)
    {
        lightCorners[i] = worldCorners[i] * LTData->LightViewMatrix;
    }

    // 6) min/max 추출
    float minX = FLT_MAX, maxX = -FLT_MAX;
    float minY = FLT_MAX, maxY = -FLT_MAX;
    float minZ = FLT_MAX, maxZ = -FLT_MAX;
    for (auto& v : lightCorners)
    {
        minX = min(minX, v.x); maxX = max(maxX, v.x);
        minY = min(minY, v.y); maxY = max(maxY, v.y);
        minZ = min(minZ, v.z); maxZ = max(maxZ, v.z);
    }

    // 7) 동적 범위 계산
    float width = maxX - minX;
    float height = maxY - minY;
    LTData->NearDistance = minZ;
    LTData->FarDistance = maxZ;
}