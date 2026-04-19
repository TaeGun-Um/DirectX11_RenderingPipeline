#pragma once
#include "Ext_Object.h"

// Component 속성을 담당하는 클래스
class Ext_Component : public Ext_Object
{
	friend class Ext_Actor;

public:
	// constrcuter destructer
	Ext_Component();
	~Ext_Component() {}

	// delete Function
	Ext_Component(const Ext_Component& _Other) = delete;
	Ext_Component(Ext_Component&& _Other) noexcept = delete;
	Ext_Component& operator=(const Ext_Component& _Other) = delete;
	Ext_Component& operator=(Ext_Component&& _Other) noexcept = delete;

	// Getter, Setter
	std::shared_ptr<class Ext_Transform> GetTransform() { return Transform; }
	std::weak_ptr<class Ext_Actor> GetOwnerActor() { return OwnerActor; }
	void SetOwnerActor(std::weak_ptr<class Ext_Actor> _Acotr) { OwnerActor = _Acotr; }

	void Destroy(bool _bIncludeChildren = false) override; // Destroy는 bIsDeath, bIsUpdate를 변경하고 루프 마지막에 지우도록 플래그만 설정하는 함수

	// 씬 레벨에 등록된 자신을 스스로 해제하는 훅.
	// - MeshComponent 는 Camera 의 MeshComponent 목록에서 빠져야 함
	// - CollisionComponent 는 Scene 의 Collisions 에서 빠져야 함
	// - 기본 구현은 no-op. 필요한 컴포넌트만 override
	// Scene/Actor Release 단계에서 죽은 컴포넌트를 제거하기 전 이 훅이 먼저 호출된다.
	// 과거에는 Scene::Release 가 dynamic_pointer_cast 로 타입을 찾아가며 직접 해제했지만,
	// 이 훅 방식으로 옮기면 RTTI 캐스트 비용이 사라지고 Component 본인이 "자기 정리"의 책임을 갖는다.
	virtual void OnDetachFromScene() {}

protected:
	virtual void Start() override;
	void Update(float _DeltaTime) override;
	virtual void Release() override;
	
	std::shared_ptr<class Ext_Transform> Transform = nullptr;	// 자신이 가진 트랜스폼 정보
	std::weak_ptr<class Ext_Actor> OwnerActor;						// 자신을 소유한 액터 정보

private:

};