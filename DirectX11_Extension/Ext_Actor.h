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
	std::shared_ptr<ComponentType> CreateComponent(std::string_view _Name)
	{
		std::shared_ptr<Ext_Component> NewComponent = std::make_shared<ComponentType>();
		std::string NewName = _Name.data();
		if (_Name == "")
		{
			const type_info& Info = typeid(ComponentType);
			NewName = Info.name();
			NewName.replace(0, 6, "");
		}

		SetComponentName(NewComponent, NewName.c_str());
		ComponentInitialize(NewComponent, this, 0);

		return std::dynamic_pointer_cast<ComponentType>(NewComponent);
	}

	std::shared_ptr<class Ext_Transform> GetTransform() { return Transform; }
	std::map<std::string, std::shared_ptr<class Ext_Component>> GetComponents() { return Components; }

protected:
	virtual void Start() override {}
	virtual void Update(float _DeltaTime) override {}
	virtual void Destroy() override {}
	
private:
	void SetComponentName(std::shared_ptr<class Ext_Component> _Component, std::string_view _Name);
	void ComponentInitialize(std::shared_ptr<class Ext_Component> _Component, std::shared_ptr<Ext_Actor> _Actor, int _Order = 0);

	std::map<std::string, std::shared_ptr<class Ext_Component>> Components; // 자신이 가진 컴포넌트들 정보
	std::shared_ptr<class Ext_Transform> Transform = nullptr; // 자신이 가진 트랜스폼 정보

};