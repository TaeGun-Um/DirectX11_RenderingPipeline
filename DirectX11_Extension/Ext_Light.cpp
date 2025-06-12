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

// ����Ʈ ���� ���� ������Ʈ, ���⼭�� Camera�� �ٶ󺸴� �ü�, ��ġ�� �ǹ���
void Ext_Light::LightUpdate(std::shared_ptr<Ext_Camera> _Camera, float _DeltaTime)
{
	LTData->CameraWorldPosition = _Camera->GetTransform()->GetWorldPosition(); // ī�޶� ���� ��ġ
	LTData->CameraViewInverseMatrix = _Camera->GetViewMatrix().InverseReturn(); // ī�޶� �� �����

	LTData->LightWorldPosition = GetTransform()->GetWorldPosition(); // �� ���� ��ġ
	LTData->LightForwardVector = GetTransform()->GetLocalForwardVector(); // �� ���� ����

	LTData->LightViewMatrix.LookToLH(GetTransform()->GetWorldPosition(), GetTransform()->GetLocalForwardVector(), GetTransform()->GetLocalUpVector()); // �� ���� �� ��� ����

    if (LTData->LightType == static_cast<int>(LightType::Directional))
    {
        //ViewPortUpdate(_Camera);
    }

	LTData->LightProjectionMatrix.OrthographicLH(ShadowRange.x, ShadowRange.y, LTData->NearDistance, LTData->FarDistance * 2.0f); // ���� ���� �������� ��� ����
	LTData->LightViewProjectionMatrix = LTData->LightViewMatrix * LTData->LightProjectionMatrix; // �� ���������� ��� ����
	LTData->LightViewInverseMatrix = LTData->LightViewMatrix.InverseReturn(); // �� �� �����
	LTData->LightProjectionInverseMatrix = LTData->LightProjectionMatrix.InverseReturn();
}

void Ext_Light::ViewPortUpdate(std::shared_ptr<Ext_Camera> _Camera)
{
    // 2) ī�޶� ����ü 8�� NDC �ڳ� ����
    std::array<float4, 8> ndc;
    ndc[0] = float4(-1, -1, 0, 1);
    ndc[1] = float4(+1, -1, 0, 1);
    ndc[2] = float4(+1, +1, 0, 1);
    ndc[3] = float4(-1, +1, 0, 1);
    ndc[4] = float4(-1, -1, 1, 1);
    ndc[5] = float4(+1, -1, 1, 1);
    ndc[6] = float4(+1, +1, 1, 1);
    ndc[7] = float4(-1, +1, 1, 1);

    // 3) ī�޶� View��Proj �����
    float4x4 camVP = _Camera->GetViewMatrix() * _Camera->GetProjectionMatrix();
    float4x4 invCamVP = camVP.InverseReturn();

    // 4) NDC �� ����
    std::vector<float4> worldCorners(8);
    for (int i = 0; i < 8; ++i)
    {
        float4 v = ndc[i] * invCamVP;
        worldCorners[i] = v / v.w;
    }

    // 5) ���� �� ����Ʈ ����
    std::vector<float4> lightCorners(8);
    for (int i = 0; i < 8; ++i)
    {
        lightCorners[i] = worldCorners[i] * LTData->LightViewMatrix;
    }

    // 6) min/max ����
    float minX = FLT_MAX, maxX = -FLT_MAX;
    float minY = FLT_MAX, maxY = -FLT_MAX;
    float minZ = FLT_MAX, maxZ = -FLT_MAX;
    for (auto& v : lightCorners)
    {
        minX = min(minX, v.x); maxX = max(maxX, v.x);
        minY = min(minY, v.y); maxY = max(maxY, v.y);
        minZ = min(minZ, v.z); maxZ = max(maxZ, v.z);
    }

    // 7) ���� ���� ���
    float width = maxX - minX;
    float height = maxY - minY;
    LTData->NearDistance = minZ;
    LTData->FarDistance = maxZ;
}