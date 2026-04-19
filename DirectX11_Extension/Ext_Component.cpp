#include "PrecompileHeader.h"
#include "Ext_Component.h"
#include "Ext_Transform.h"
#include "Ext_Actor.h"

Ext_Component::Ext_Component()
{
	Transform = std::make_shared<Ext_Transform>();
}

// Destroy — 실제 제거가 아니라 "죽음 예약". 플래그만 세우고 Actor 의 PendingDeadComponents 에 등록
// - 순회 중 iterator 무효화를 피하려는 지연 삭제 패턴
// - 실제 Release 는 Actor::Update 의 다음 프레임에 RemoveDeadComponents() 에서 수행
void Ext_Component::Destroy(bool _bIncludeChildren/* = false*/)
{
	if (bIsDeath) return;

	bIsDeath  = true;
	bIsUpdate = false;

	// [1] 자신 보존 (shared_from_this 가 유효하려면 이 객체가 어딘가의 shared_ptr 에 있어야 함)
	std::shared_ptr<Ext_Component> SelfKeepAlive = GetSharedFromThis<Ext_Component>();

	// [2] 재귀 옵션 — 컴포넌트 트랜스폼 자식들도 Destroy 예약
	//     부모가 죽으면 자식도 같이 정리되는 게 대부분 자연스럽지만,
	//     특정 케이스(카메라 분리 등)를 위해 옵션으로 분리해 둠
	if (_bIncludeChildren)
	{
		std::shared_ptr<Ext_Transform> TF = GetTransform();
		if (TF)
		{
			const auto& Children = TF->GetChildren();
			for (const auto& ChildTransform : Children)
			{
				if (auto ChildComp = ChildTransform->GetOwnerComponent().lock())
				{
					ChildComp->Destroy(true); // 재귀 Destroy
				}
			}
		}
	}

	// [3] Actor 에 "나 죽었어요" 등록 → Actor 가 다음 Update 에서 Release + Components 맵에서 제거
	if (std::shared_ptr<Ext_Actor> Owner = GetOwnerActor().lock())
	{
		Owner->MarkComponentDead(SelfKeepAlive);
	}
}

void Ext_Component::Release()
{
	// [1] Transform 정리
	if (Transform)
	{
		Transform->Release(); // Transform 계층에서 자신 정리 + 부모/자식 관계 끊기
	}
	Transform = nullptr;

	// [2] 참조 정보 해제
	OwnerActor.reset();
	OwnerScene.reset();

	// [3] 기타 자원 정리 필요 시 확장 지점
}

void Ext_Component::Start()
{

}

void Ext_Component::Update(float _DeltaTime)
{

}
