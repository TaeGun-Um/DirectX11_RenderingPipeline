#pragma once
#include "Ext_Object.h"

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
	std::shared_ptr<class Ext_Actor> GetOwnerActor() { return OwnerActor; }
	void SetOwnerActor(std::shared_ptr<class Ext_Actor> _Acotr) { OwnerActor = _Acotr; }

protected:
	virtual void Start() {}
	virtual void Update(float _DeltaTime) {}
	virtual void Destroy() {}
	
private:
	std::shared_ptr<class Ext_Transform> Transform = nullptr;
	std::shared_ptr<class Ext_Actor> OwnerActor = nullptr;
};