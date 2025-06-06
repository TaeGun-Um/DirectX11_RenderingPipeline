﻿#pragma once
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

	void MarkDeadComponent() {	bHasDeadComponent = true; } // 지우기 위한 플래그 설정

protected:
	virtual void Start() override {}
	virtual void Update(float _DeltaTime);
	virtual void Release() override;
	void RemoveDeadComponents(); // 특정 컴포넌트만 제거
	void ComponentInitialize(std::shared_ptr<class Ext_Component> _Component, std::weak_ptr<Ext_Actor> _Actor, std::string_view _Name, int _Order); 
	
	std::map<std::string, std::shared_ptr<class Ext_Component>> Components; // 자신이 가진 컴포넌트들 정보
	std::shared_ptr<class Ext_Transform> Transform = nullptr; // 자신이 가진 트랜스폼 정보

	bool bHasDeadComponent = false; // 죽은 컴포넌트 존재 여부

private:

};