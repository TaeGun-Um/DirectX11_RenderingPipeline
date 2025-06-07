#include "PrecompileHeader.h"
#include "Ext_Actor.h"
#include "Ext_Component.h"
#include "Ext_Transform.h"

Ext_Actor::Ext_Actor()
{
	Transform = std::make_shared<Ext_Transform>();
}

void Ext_Actor::RemoveDeadComponents()
{
	for (auto Iter = Components.begin(); Iter != Components.end(); )
	{
		if (Iter->second && Iter->second->IsDeath())
		{
			Iter->second->Release(); // 컴포넌트 자원 정리
			Iter = Components.erase(Iter);
		}
		else
		{
			++Iter;
		}
	}
}

void Ext_Actor::Release()
{
	Destroy();

	// [1] 자신이 보유한 모든 Component를 제거 요청 (재귀적으로 자식까지 제거됨)
	for (auto& [Name, Component] : Components)
	{
		if (Component)
		{
			Component->Destroy();
		}
	}

	// [2] 논리적 제거
	Components.clear();
	OwnerScene.reset();

	// [3] Actor가 Transform을 소유한 경우, 직접 정리
	if (Transform)
	{
		Transform->Release();
		Transform = nullptr;
	}
}

void Ext_Actor::Update(float _DeltaTime)
{
	std::string Name = GetName();

	if (bHasDeadComponent)
	{
		RemoveDeadComponents(); // 죽은 컴포넌트만 제거
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
