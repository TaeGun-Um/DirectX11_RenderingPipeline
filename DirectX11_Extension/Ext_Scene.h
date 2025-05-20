#pragma once
#include "Ext_Object.h"

class Ext_Scene : public Ext_Object
{
	friend class Ext_Core;

public:
	// constrcuter destructer
	Ext_Scene();
	~Ext_Scene();

	// delete Function
	Ext_Scene(const Ext_Scene& _Other) = delete;
	Ext_Scene(Ext_Scene&& _Other) noexcept = delete;
	Ext_Scene& operator=(const Ext_Scene& _Other) = delete;
	Ext_Scene& operator=(Ext_Scene&& _Other) noexcept = delete;

	// Actor 생성 및 저장
	template<typename ActorType>
	std::shared_ptr<ActorType> CreateActor(std::string_view _Name)
	{
		std::shared_ptr<Ext_Actor> NewActor = std::make_shared<ActorType>();
		std::string NewName = _Name.data();
		if (_Name == "")
		{
			const type_info& Info = typeid(ActorType);
			NewName = Info.name();
			NewName.replace(0, 6, "");
		}

		SetActorName(NewActor, NewName.c_str());
		ActorInitialize(NewActor, this, 0);

		return std::dynamic_pointer_cast<ActorType>(NewActor);
	}

protected:
	virtual void Start() {}
	virtual void Update(float _DeltaTime) {}
	virtual void Destroy() {}
	
private:
	void SetActorName(std::shared_ptr<class Ext_Actor> _Actor, std::string_view _Name);
	void ActorInitialize(std::shared_ptr<class Ext_Actor> _Actor, std::shared_ptr<Ext_Scene> _Level, int _Order = 0);

	std::map<std::string, std::shared_ptr<class Ext_Actor>> Actors;
};