#include "PrecompileHeader.h"
#include "CubeActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>

CubeActor::CubeActor()
{
}

CubeActor::~CubeActor()
{
}

void CubeActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh"); // 메시 생성
	MeshComp->CreateMeshComponentUnit("Box", "Basic"); // 렌더링 세팅 설정
	//MeshComp->GetTransform()->SetWorldPosition({ 0, 0, 10});
	GetTransform()->SetLocalPosition({ 0, 0, 4 });

	MeshComp2 = CreateComponent<Ext_MeshComponent>("BasicMesh2"); // 메시 생성;
	MeshComp2->CreateMeshComponentUnit("Box", "Basic"); // 렌더링 세팅 설정
	MeshComp2->GetTransform()->SetParent(GetTransform());
	MeshComp2->GetTransform()->SetLocalPosition({ 2.f, 0.f, 0.f });

	//PivotComponent = OrbitPivot;
}

void CubeActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	float MoveSpeed = 100.0f; // 초당 10 단위 회전
	// 
	MeshComp->GetTransform()->AddLocalRotation({ MoveSpeed * _DeltaTime, 0.f, 0.f });
	MeshComp->GetTransform()->AddLocalRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
	MeshComp->GetTransform()->AddLocalRotation({ 0.f, 0.f, MoveSpeed * _DeltaTime });

	// 원형 궤도 회전 (Y축 기준 회전)
	//float Speed = 90.0f; // 1초에 90도
	//float4 Rotation = PivotComponent->GetTransform()->GetWorldRotation();
	//Rotation.y += Speed * _DeltaTime;
	//PivotComponent->GetTransform()->SetWorldRotation(Rotation);

	AccTime += _DeltaTime;
	if (AccTime >= 3.f)
	{
		//Destroy();
		MeshComp2->Destroy(true);
	}
}