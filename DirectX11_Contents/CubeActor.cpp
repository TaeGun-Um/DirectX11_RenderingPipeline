#include "PrecompileHeader.h"
#include "CubeActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

CubeActor::CubeActor()
{
}

CubeActor::~CubeActor()
{
}

void CubeActor::Start()
{
	GetTransform()->SetWorldPosition({ 0.f, 0.f, 20.0f }); // 위치 설정
	GetTransform()->SetWorldScale({ 10.f, 10.f, 10.f }); // 크기 설정
	std::shared_ptr<Ext_MeshComponent> MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh", true); // 메시 생성
	MeshComp->CreateMeshComponentUnit("Box", "Basic"); // 렌더링 세팅 설정
}

void CubeActor::Update(float _DeltaTime)
{
	float MoveSpeed = 100.0f; // 초당 10 단위 회전

	GetTransform()->AddWorldRotation({ MoveSpeed * _DeltaTime, 0.f, 0.f });
	GetTransform()->AddWorldRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
	GetTransform()->AddWorldRotation({ 0.f, 0.f, MoveSpeed * _DeltaTime });

	AccTime += _DeltaTime;
	if (AccTime >= 2.0f)
	{
		SetIsDeath(true);
	}
}