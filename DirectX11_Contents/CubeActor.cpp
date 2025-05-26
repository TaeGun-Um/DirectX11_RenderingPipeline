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

	// 부모 설정 제거 → 독립적인 위치 갱신
	MeshComp2->GetTransform()->SetParent(GetTransform());

	AccTime = 0.0f;
	RotationDegree = 0.0f;
}

void CubeActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	// 자기 자신 회전
	float MoveSpeed = 100.0f;
	float MoveSpeed2 = 10.0f;
	//GetTransform()->AddLocalRotation({ MoveSpeed * _DeltaTime, MoveSpeed * _DeltaTime, MoveSpeed * _DeltaTime });
	GetTransform()->AddLocalPosition({ MoveSpeed2 * _DeltaTime * 0.1f, 0.f, 0.f });

	// 공전 로직
	RotationDegree += 180.0f * _DeltaTime; // 초당 90도 회전
	if (RotationDegree >= 360.0f)
	{
		RotationDegree -= 360.0f;
	}

	float Radius = 2.f;
	float Radian = RotationDegree * Base_Math::PIE / 180.0f;
	float4 Offset = float4(cosf(Radian) * Radius, 0.0f, sinf(Radian) * Radius);

	// ✅ 월드가 아닌 로컬 위치로 설정
	MeshComp2->GetTransform()->SetLocalPosition(Offset);
}