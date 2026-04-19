#include "PrecompileHeader.h"
#include "Ext_Actor.h"
#include "Ext_Component.h"
#include "Ext_Transform.h"
#include "Ext_Scene.h"

Ext_Actor::Ext_Actor()
{
	Transform = std::make_shared<Ext_Transform>();
}

// Destroy — 실제 제거가 아니라 "죽었음" 표시 + Scene·Actor 에 정리 대기 등록
// - 이 함수는 매 프레임 Update 루프 중 어디서든 호출될 수 있음.
// - 그래서 즉시 Release 하지 않고 플래그만 세운 뒤, 프레임 끝 Scene::Release 에서 안전하게 정리한다.
// - 반복 Destroy 호출에 대비해 bIsDeath 로 가드
// - Scene 의 PendingDeadActors 에 자신을 등록 → Scene 전체 순회 없이 해당 프레임에 빠르게 정리 가능
void Ext_Actor::Destroy(bool _bIncludeChildren /*= false*/)
{
	if (bIsDeath) return;

	bIsDeath  = true;
	bIsUpdate = false;

	// [1] 자식 옵션: 보유 Component 들까지 같이 Destroy 예약
	//     재귀 파라미터를 true 로 넘겨서 MeshComponent 하위 트랜스폼 자식 등도 같이 정리되도록 함
	if (_bIncludeChildren)
	{
		for (auto& [Name, Comp] : Components)
		{
			if (Comp && !Comp->IsDeath())
			{
				Comp->Destroy(true);
			}
		}
	}

	// [2] Scene 에 "나 죽었어요" 등록
	//     - shared_from_this 는 이 Actor 가 어딘가의 shared_ptr 에 담겨 있을 때만 유효 (CreateActor 경로 사용 시 항상 그럼)
	if (std::shared_ptr<Ext_Scene> Scene = GetOwnerScene().lock())
	{
		Scene->MarkDeadActor(GetSharedFromThis<Ext_Actor>());
	}
}

// PendingDeadComponents 에 쌓인 컴포넌트들만 정리
// - 기존에는 Components 맵 전체를 iterate 하며 IsDeath() 로 찾아냈음 → O(전체) 비용
// - 이제는 MarkComponentDead 에서 등록한 것만 순회 → O(dead) 비용
void Ext_Actor::RemoveDeadComponents()
{
	if (PendingDeadComponents.empty()) return;

	for (auto& DeadComp : PendingDeadComponents)
	{
		if (!DeadComp) continue;

		// [1] 씬 레벨 등록(카메라 MeshComp 리스트 등)에서 먼저 빠지기
		//     MeshComponent 같은 건 OnDetachFromScene 오버라이드에서 카메라 제거까지 책임
		DeadComp->OnDetachFromScene();

		// [2] 컴포넌트 자체 자원 정리
		DeadComp->Release();

		// [3] Components 맵에서 제거 (이름으로 erase)
		const std::string& Name = DeadComp->GetName();
		Components.erase(Name);
	}

	PendingDeadComponents.clear();
}

// 컴포넌트의 Destroy 가 호출되면 이 메서드로 와서 리스트에 쌓임
// - 이미 죽은 상태로 중복 요청된 경우 리스트에는 넣지 않음 (위에서 IsDeath 체크는 Component::Destroy 에서 수행)
// - 플래그도 같이 세워 Actor::Update 가 다음 프레임에 RemoveDeadComponents() 진입 결정 가능하게 함
void Ext_Actor::MarkComponentDead(std::shared_ptr<Ext_Component> _Component)
{
	if (!_Component) return;

	PendingDeadComponents.push_back(_Component);
	bHasDeadComponent = true;
}

// Actor 자원 전체 정리 — Scene::Release 에서 PendingDeadActors 순회 중 호출
// - Destroy() 와 혼동 주의: Destroy 는 "죽음 예약", Release 는 "실제 정리"
void Ext_Actor::Release()
{
	// [1] 보유 중인 모든 Component 에 Destroy 예약
	//     - 아직 Destroy 되지 않은 컴포넌트들이 남아 있을 수 있음
	//     - 여기서는 Destroy 예약만 하고, Release 자체는 아래 루프에서 일괄 처리
	for (auto& [Name, Comp] : Components)
	{
		if (Comp && !Comp->IsDeath())
		{
			Comp->Destroy();
		}
	}

	// [2] 남은 컴포넌트들을 직접 Release (PendingDeadComponents 경유 없이 즉시 정리)
	for (auto& [Name, Comp] : Components)
	{
		if (!Comp) continue;
		Comp->OnDetachFromScene();
		Comp->Release();
	}

	// [3] 자료구조 정리
	Components.clear();
	PendingDeadComponents.clear();
	OwnerScene.reset();

	// [4] Transform 정리
	if (Transform)
	{
		Transform->Release();
		Transform = nullptr;
	}
}

void Ext_Actor::Update(float _DeltaTime)
{
	// 지난 프레임에 Destroy 된 컴포넌트가 있으면 정리 — fast path 가드
	if (bHasDeadComponent)
	{
		RemoveDeadComponents();
		bHasDeadComponent = false;
	}

	// [1] 보유 컴포넌트들 Update 호출
	for (const auto& [Name, Comp] : Components)
	{
		if (Comp && !Comp->IsDeath())
		{
			Comp->Update(_DeltaTime);
		}
	}

	// [2] Actor 자체 로직 수행 (필요 시 가상함수 또는 람다로 분리 가능)
	//OnUpdate(_DeltaTime);
}

// 컴포넌트 생성 시 자동호출(오너 액터 설정, 오너 신 설정, 이름 저장, 오더 저장)
void Ext_Actor::ComponentInitialize(std::shared_ptr<Ext_Component> _Component, std::weak_ptr<Ext_Actor> _Actor, std::string_view _Name, int _Order /*=0*/)
{
	_Component->SetOwnerActor(_Actor);
	_Component->SetOwnerScene(GetOwnerScene());
	_Component->SetName(_Name);
	_Component->SetOrder(_Order);
	_Component->Start(); // ✅ Transform 생성 이후

	if (std::shared_ptr<Ext_Transform> CompTransform = _Component->GetTransform())
	{
		CompTransform->SetOwnerComponent(_Component);
		CompTransform->SetParent(GetTransform()); // ✅ 부모 설정
	}
}
