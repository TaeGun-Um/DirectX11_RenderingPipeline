#pragma once
#include "Ext_Object.h"

// Actor 속성을 담당하는 클래스
class Ext_Actor : public Ext_Object
{
	friend class Ext_Scene;

public:
	// constrcuter destructer
	Ext_Actor();
	~Ext_Actor() {}

	// delete Function
	Ext_Actor(const Ext_Actor& _Other) = delete;
	Ext_Actor(Ext_Actor&& _Other) noexcept = delete;
	Ext_Actor& operator=(const Ext_Actor& _Other) = delete;
	Ext_Actor& operator=(Ext_Actor&& _Other) noexcept = delete;

	// 충돌체 컴포넌트 생성 후 저장
	template<typename ComponentType>
	std::shared_ptr<ComponentType> CreateComponent(std::string_view _Name, CollisionGroup _Group)
	{
		CreateComponent(_Name, static_cast<int>(_Group));
	}

	// 컴포넌트 생성 후 저장
	template<typename ComponentType>
	std::shared_ptr<ComponentType> CreateComponent(std::string_view _Name, int _Order = 0)
	{
		if (_Name == "")
		{
			MsgAssert("Component 생성 시에는 이름을 지정해주세요");
			return nullptr;
		}

		std::shared_ptr<Ext_Component> NewComponent = std::make_shared<ComponentType>();
		std::string NewName = _Name.data();

		if (Components.find(NewName) != Components.end())
		{
			MsgAssert("해당 Actor 내에 이미 동일한 이름의 Component가 존재합니다.");
			return nullptr;
		}

		ComponentInitialize(NewComponent, GetSharedFromThis<Ext_Actor>(), NewName.c_str(), _Order);
		Components.insert(std::make_pair(NewName, NewComponent));

		return std::dynamic_pointer_cast<ComponentType>(NewComponent);
	}

	// Component 찾기
	template<typename ComponentType>
	std::shared_ptr<ComponentType> FindComponent(std::string_view _Name)
	{
		std::string Name = _Name.data();

		if (Components.end() == Components.find(Name))
		{
			MsgAssert("없는 Scene은 찾을 수 없습니다.");
			return nullptr;
		}

		return std::dynamic_pointer_cast<ComponentType>(Components[Name]);
	}

	// Getter, Setter
	std::shared_ptr<class Ext_Transform> GetTransform() { return Transform; }
	std::map<std::string, std::shared_ptr<class Ext_Component>> GetComponents() { return Components; }

	// 죽은 컴포넌트를 전용 리스트에 등록하고 플래그 설정
	// - 기존에는 bHasDeadComponent 플래그만 세우고 Actor::Update 에서 Components 전체를 순회했지만,
	//   이제는 "누가 죽었는지"를 명시적으로 리스트에 쌓아 다음 Update 때 해당 항목만 O(dead) 로 정리
	// - 중복 호출 방지를 위해 이미 Death 상태인지 체크
	void MarkComponentDead(std::shared_ptr<class Ext_Component> _Component);

	// Destroy 는 플래그만 세우고 실제 제거는 다음 Scene::Release 에서 처리
	// - 자식 컴포넌트 재귀 옵션 (기본은 false). Character 처럼 자식 계층이 큰 경우 true 로 호출
	// - Scene 에도 Dead 등록 요청 → Scene::Release 가 PendingDeadActors 만 빠르게 순회하도록
	void Destroy(bool _bIncludeChildren = false) override;

protected:
	virtual void Start() override {}
	virtual void Update(float _DeltaTime);
	virtual void Release() override;

	// PendingDeadComponents 에 쌓인 항목들만 선별적으로 Release + 맵에서 erase
	// - 과거에는 Components 맵 전체 순회 후 IsDeath() 체크로 찾았음
	// - 지금은 "죽은 것 목록" 에 직접 접근하므로 죽은 개수에 비례한 비용만 듬
	void RemoveDeadComponents();

	void ComponentInitialize(std::shared_ptr<class Ext_Component> _Component, std::weak_ptr<Ext_Actor> _Actor, std::string_view _Name, int _Order);

	std::map<std::string, std::shared_ptr<class Ext_Component>> Components; // 자신이 가진 컴포넌트들 정보
	std::shared_ptr<class Ext_Transform> Transform = nullptr; // 자신이 가진 트랜스폼 정보

	// Destroy 된 컴포넌트들을 다음 정리 시점까지 잠시 보관
	// (원본 Components 맵에서 즉시 지우면 현재 순회 중인 Update 루프가 iterator 무효화로 죽을 수 있음 → 지연 삭제 패턴)
	std::vector<std::shared_ptr<class Ext_Component>> PendingDeadComponents;

	bool bHasDeadComponent = false; // 죽은 컴포넌트 존재 여부 — fast path 가드

private:

};