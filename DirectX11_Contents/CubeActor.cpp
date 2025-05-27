#include "PrecompileHeader.h"
#include "CubeActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>
#include <DirectX11_Base/Base_Math.h>

void CubeActor::Start()
{
	GetTransform()->SetLocalPosition({ 0, 0, 5 });

	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Box", "Basic");

	MeshComp2 = CreateComponent<Ext_MeshComponent>("BasicMesh2");
	MeshComp2->CreateMeshComponentUnit("Box", "Basic");
	MeshComp2->GetTransform()->SetLocalPosition({ 3, 0, 0 });
}

void CubeActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	float Radius = 2.f;
	float Radian = RotationDegree * Base_Math::PIE / 180.0f;
	float4 Offset = float4(cosf(Radian) * Radius, 0.0f, sinf(Radian) * Radius);

	RotationDegree += 180.0f * _DeltaTime; // 초당 90도 회전
	if (RotationDegree >= 360.0f)
	{
		RotationDegree -= 360.0f;
	}

	// ✅ 월드가 아닌 로컬 위치로 설정
	MeshComp2->GetTransform()->SetLocalPosition(Offset);

}