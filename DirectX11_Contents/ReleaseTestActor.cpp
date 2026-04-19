#include "PrecompileHeader.h"
#include "ReleaseTestActor.h"

#include <DirectX11_Base/Base_Math.h>
#include <DirectX11_Extension/Ext_Scene.h>
#include <DirectX11_Extension/Ext_Transform.h>

#include "CubeActor.h"
#include "SphereActor.h"

void ReleaseTestActor::Start()
{
	AccTime      = 0.0f;
	CycleCounter = 0;
}

float TestTime = 0.f;

// 매 프레임 호출
// - AccTime 이 CyclePeriod 를 넘으면 한 사이클 완료:
//     1) 기존 배치 전원 Destroy (죽음 예약 → Scene::Release 에서 실제 정리)
//     2) 새 배치 스폰 (Cube/Sphere 교대 배치)
// - 사이클이 돌수록 CycleCounter 증가 → Actor 이름 중복 방지
void ReleaseTestActor::Update(float _DeltaTime)
{
	Ext_Actor::Update(_DeltaTime);

	AccTime += _DeltaTime;
	if (AccTime < CyclePeriod) return;

	AccTime = 0.0f;
	++CycleCounter;

	std::shared_ptr<Ext_Scene> Scene = GetOwnerScene().lock();
	if (nullptr == Scene) return;

	TestTime += _DeltaTime;
	if (TestTime <= 1.0f) return;
	TestTime = 0.f;

	// ---- [1] 이전 배치 Destroy ----
	// weak_ptr 로 들고 있으므로 이미 사라진 것은 lock() 결과 null 로 자동 걸러짐.
	// Destroy 는 플래그만 세우고 Scene 의 PendingDeadActors 에 등록 → 같은 프레임 말에 Scene::Release 가 정리.
	for (std::weak_ptr<Ext_Actor>& Weak : CurrentBatch)
	{
		if (auto Alive = Weak.lock())
		{
			Alive->Destroy();
		}
	}
	CurrentBatch.clear();

	// ---- [2] 새 배치 스폰 ----
	// 자기 World 위치를 중심으로 SpawnCount 개를 원형으로 배치
	const float4 CenterPos = GetTransform()->GetWorldPosition();
	const float  TwoPi     = Base_Math::PIE2;

	for (int i = 0; i < SpawnCount; ++i)
	{
		// i 번째 슬롯의 원주각
		const float Angle = (static_cast<float>(i) / static_cast<float>(SpawnCount)) * TwoPi;
		const float OffsetX = cosf(Angle) * SpawnRadius;
		const float OffsetZ = sinf(Angle) * SpawnRadius;
		const float4 PlacePos = { CenterPos.x + OffsetX, CenterPos.y, CenterPos.z + OffsetZ, 1.0f };

		// 짝/홀수로 Cube / Sphere 교대 → 시각적으로 구분 쉬움
		std::string Name = "RTA_" + std::to_string(CycleCounter) + "_" + std::to_string(i);
		std::shared_ptr<Ext_Actor> NewActor;

		if (0 == (i & 1))
		{
			std::shared_ptr<CubeActor> Cube = Scene->CreateActor<CubeActor>(Name);
			Cube->GetTransform()->SetLocalPosition(PlacePos);
			Cube->GetTransform()->SetLocalScale({ 25.f, 25.f, 25.f });   // 본체보다 작게
			NewActor = Cube;
		}
		else
		{
			std::shared_ptr<SphereActor> Sphere = Scene->CreateActor<SphereActor>(Name);
			Sphere->GetTransform()->SetLocalPosition(PlacePos);
			Sphere->GetTransform()->SetLocalScale({ 25.f, 25.f, 25.f });
			NewActor = Sphere;
		}

		CurrentBatch.push_back(NewActor);
	}
}
