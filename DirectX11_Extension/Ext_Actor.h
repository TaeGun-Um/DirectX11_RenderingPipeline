#pragma once
#include "Ext_Object.h"

// Actor 속성을 담당하는 클래스
class Ext_Actor : public Ext_Object
{
	friend class Ext_Scene;

public:
	// constrcuter destructer
	Ext_Actor();
	~Ext_Actor();

	// delete Function
	Ext_Actor(const Ext_Actor& _Other) = delete;
	Ext_Actor(Ext_Actor&& _Other) noexcept = delete;
	Ext_Actor& operator=(const Ext_Actor& _Other) = delete;
	Ext_Actor& operator=(Ext_Actor&& _Other) noexcept = delete;

	// 컴포넌트 생성 후 저장
	template<typename ComponentType>
	std::shared_ptr<ComponentType> CreateComponent(std::string_view _Name, int _Order = 0, bool _IsTransformShare = false)
	{
		std::shared_ptr<Ext_Component> NewComponent = std::make_shared<ComponentType>();
		std::string NewName = _Name.data();
		if (_Name == "")
		{
			const type_info& Info = typeid(ComponentType);
			NewName = Info.name();
			NewName.replace(0, 6, "");
		}

		ComponentInitialize(NewComponent, GetSharedFromThis<Ext_Actor>(), NewName.c_str(), _IsTransformShare);
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

protected:
	virtual void Start() override {}
	virtual void Update(float _DeltaTime) override {}
	virtual void Destroy() override {}
	void ComponentInitialize(std::shared_ptr<class Ext_Component> _Component, std::weak_ptr<Ext_Actor> _Actor, std::string_view _Name, int _Order, bool __IsTransformShare = false); 
	
	std::map<std::string, std::shared_ptr<class Ext_Component>> Components; // 자신이 가진 컴포넌트들 정보
	std::shared_ptr<class Ext_Transform> Transform = nullptr; // 자신이 가진 트랜스폼 정보

private:

};