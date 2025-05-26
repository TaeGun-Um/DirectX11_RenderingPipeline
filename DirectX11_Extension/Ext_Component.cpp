#include "PrecompileHeader.h"
#include "Ext_Component.h"
#include "Ext_Transform.h"
#include "Ext_Actor.h"

Ext_Component::Ext_Component()
{
	Transform = std::make_shared<Ext_Transform>();
}

// Destroy는 bIsDeath, bIsUpdate를 변경하고 루프 마지막에 지우도록 플래그만 설정하는 함수
void Ext_Component::Destroy(bool _bIncludeChildren/* = false*/)
{
	if (bIsDeath) return;
	bIsDeath = true;
	bIsUpdate = false;

	// [1] 자신 보존
	std::shared_ptr<Ext_Component> SelfKeepAlive = GetSharedFromThis<Ext_Component>();

	// [2] 재귀 제거 옵션이 true인 경우에만 자식 제거
	if (_bIncludeChildren)
	{
		auto Transform = GetTransform(); // 컴포넌트가 재귀적으로 지워질 때는 트랜스폼의 부모자식 관계를 확인하여 지움
		if (Transform)
		{
			const auto& Children = Transform->GetChildren(); // 자식 있나요?
			for (const auto& ChildTransform : Children)
			{
				if (auto ChildComp = ChildTransform->GetOwnerComponent().lock())
				{
					ChildComp->Destroy(true); // 재귀로 자식도 제거
				}
			}
		}
	}

	// [3] Actor에 제거 요청
	if (auto Owner = GetOwnerActor().lock())
	{
		Owner->MarkDeadComponent();
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
