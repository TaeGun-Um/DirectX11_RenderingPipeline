#include "PrecompileHeader.h"
#include "Ext_Actor.h"
#include "Ext_Component.h"
#include "Ext_Transform.h"

Ext_Actor::Ext_Actor()
{
	Transform = std::make_shared<Ext_Transform>();
}

Ext_Actor::~Ext_Actor()
{
}

void Ext_Actor::Destroy()
{
	// [1] 모든 컴포넌트 Destroy 호출
	for (auto& [Name, Component] : Components)
	{
		if (Component)
		{
			Component->Destroy(); // 개별 컴포넌트 정리
		}
	}
	Components.clear();

	// [2] Transform 제거
	if (Transform)
	{
		Transform->Destroy(); // TransformData 초기화
		Transform = nullptr;
	}

	// [3] 필요 시 MeshComponentUnit 등 부가 리소스도 해제
	// 예: MeshUnits.clear();

	// [4] 기타 사용자 정의 자원 해제
	// 예: 이벤트 핸들러 해제, 태그 초기화, 캐시된 포인터 null 처리 등
}

// 컴포넌트 생성 시 자동호출(오너 액터 설정, 오너 신 설정, 이름 저장, 오더 저장)
void Ext_Actor::ComponentInitialize(std::shared_ptr<Ext_Component> _Component, std::weak_ptr<Ext_Actor> _Actor, std::string_view _Name, int _Order /*=0*/, bool __IsTransformShare /*= false*/)
{
	_Component->SetOwnerActor(_Actor);
	_Component->SetOwnerScene(GetOwnerScene());
	_Component->IsTransformShare = __IsTransformShare;
	_Component->SetName(_Name);
	_Component->SetOrder(_Order);
	_Component->Start();
}
