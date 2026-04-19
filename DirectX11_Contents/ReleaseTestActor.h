#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

// Release 구조 시각화 테스트용 액터
// - 1초 간격으로 주변에 자식 Actor 10개를 스폰하고, 바로 다음 주기에서 Destroy
// - 프레임워크의 Destroy → PendingDeadActors → Scene::Release 경로를 눈으로 확인하기 위한 도구
// - 캐릭터 옆이나 원하는 위치에 1개만 배치해 두면 계속 순환 동작
class ReleaseTestActor : public Ext_Actor
{
public:
	ReleaseTestActor() = default;
	~ReleaseTestActor() = default;

	ReleaseTestActor(const ReleaseTestActor&) = delete;
	ReleaseTestActor(ReleaseTestActor&&) noexcept = delete;
	ReleaseTestActor& operator=(const ReleaseTestActor&) = delete;
	ReleaseTestActor& operator=(ReleaseTestActor&&) noexcept = delete;

	// 스폰 반경(자기 중심으로 원형 배치) — 기본 150
	void SetSpawnRadius(float _Radius) { SpawnRadius = _Radius; }

	// 사이클 주기(초) — 기본 1.0
	void SetCyclePeriod(float _Seconds) { CyclePeriod = _Seconds; }

	// 한 사이클당 스폰 개수 — 기본 10
	void SetSpawnCount(int _Count)      { SpawnCount = _Count; }

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	// 현재 살아 있는 배치. Actor 들의 수명은 Scene 이 보유하므로 여기서는 weak_ptr 로만 추적
	// → 다음 사이클에서 Destroy 호출만 하면 실제 정리는 Scene::Release 가 처리
	std::vector<std::weak_ptr<Ext_Actor>> CurrentBatch;

	float AccTime      = 0.0f; // 사이클 누적 시간
	int   CycleCounter = 0;    // 몇 번째 사이클인지 — 이름 중복 회피용

	// 튜닝 파라미터
	float SpawnRadius = 150.0f;
	float CyclePeriod = 1.0f;
	int   SpawnCount  = 10;
};
