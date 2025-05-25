#include "PrecompileHeader.h"
#include "Ext_Component.h"
#include "Ext_Transform.h"
#include "Ext_Actor.h"

Ext_Component::Ext_Component()
{
	Transform = std::make_shared<Ext_Transform>();
}

void Ext_Component::SetIsDeath(bool Value, bool bIncludeChildren/* = false*/)
{
	bIsDeath = Value;

	if (!bIsDeath)
	{
		return;
	}

	// [1] 자신 보존
	std::shared_ptr<Ext_Component> SelfKeepAlive = GetSharedFromThis<Ext_Component>();

	// [2] 재귀 제거 옵션이 true인 경우에만 자식 제거
	if (bIncludeChildren)
	{
		auto Transform = GetTransform();
		if (Transform)
		{
			const auto& Children = Transform->GetChildren();
			for (const auto& ChildTransform : Children)
			{
				if (auto ChildComp = ChildTransform->GetOwnerComponent().lock())
				{
					ChildComp->SetIsDeath(true, true); // 재귀로 자식도 제거
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

void Ext_Component::Destroy()
{
	// [1] Transform 정리
	if (Transform)
	{
		Transform->Destroy(); // Transform 계층에서 자신 정리 + 부모/자식 관계 끊기
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
