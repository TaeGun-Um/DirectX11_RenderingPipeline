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
	Ext_Actor::Start();
	// CreateComponent<Ext_Component>("TestComp", true);
	GetTransform()->SetWorldPosition({ 150.f, 0.f, 100.0f });
	GetTransform()->SetWorldScale({ 100.f, 100.f, 100.f }); // 크기 확대
	std::shared_ptr<Ext_MeshComponent> MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh", true);
	MeshComp->CreateMeshComponentUnit("Box", "Basic");
}

void CubeActor::Update(float _DeltaTime)
{
	static bool bUp = true;

	AccTime += _DeltaTime;

	float MoveSpeed = 100.0f; // 초당 100 단위 이동
	float4 MoveDir = float4::ZERO;

	if (bUp)
	{
		MoveDir = float4::UP * MoveSpeed * _DeltaTime;
	}
	else
	{
		MoveDir = float4::DOWN * MoveSpeed * _DeltaTime;
	}

	GetTransform()->AddWorldPosition(MoveDir);
	GetTransform()->AddWorldRotation({ 1.f, 0.f, 0.f });
	GetTransform()->AddWorldRotation({ 0.f, 1.f, 0.f });
	GetTransform()->AddWorldRotation({ 0.f, 0.f, 1.f });

	if (AccTime >= 1.5f)
	{
		AccTime = 0.0f;
		bUp = !bUp; // 방향 반전
	}
}

void CubeActor::Destroy()
{

}