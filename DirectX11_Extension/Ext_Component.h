#pragma once
#include "Ext_Object.h"

// Component �Ӽ��� ����ϴ� Ŭ����
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
	
	std::shared_ptr<class Ext_Transform> Transform = nullptr; // �ڽ��� ���� Ʈ������ ����
	std::weak_ptr<class Ext_Actor> OwnerActor; // �ڽ��� ������ ���� ����
	bool IsTransformShare = false; // ���� �� Ʈ�������� Actor�� ������ ������ ����

private:

};