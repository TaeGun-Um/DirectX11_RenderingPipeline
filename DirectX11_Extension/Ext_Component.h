#pragma once
#include "Ext_Object.h"

// Component 속성을 담당하는 클래스
class Ext_Component : public Ext_Object
{
	friend class Ext_Actor;

public:
	// constrcuter destructer
	Ext_Component();
	~Ext_Component();

	// delete Function
	Ext_Component(const Ext_Component& _Other) = delete;
	Ext_Component(Ext_Component&& _Other) noexcept = delete;
	Ext_Component& operator=(const Ext_Component& _Other) = delete;
	Ext_Component& operator=(Ext_Component&& _Other) noexcept = delete;

	// Getter, Setter
	std::shared_ptr<class Ext_Transform> GetTransform() { return Transform; }
	std::weak_ptr<class Ext_Actor> GetOwnerActor() { return OwnerActor; }
	void SetOwnerActor(std::weak_ptr<class Ext_Actor> _Acotr) { OwnerActor = _Acotr; }

protected:
	virtual void Start() override;
	virtual void Update(float _DeltaTime) override {}
	virtual void Destroy() override {}
	
	std::shared_ptr<class Ext_Transform> Transform = nullptr; // 자신이 가진 트랜스폼 정보
	std::weak_ptr<class Ext_Actor> OwnerActor; // 자신을 소유한 액터 정보
	bool IsTransformShare = false; // 생성 시 트랜스폼을 Actor와 공유할 것인지 설정

private:

};