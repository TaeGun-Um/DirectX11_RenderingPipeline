#pragma once
#include "Ext_Object.h"

// Scene �Ӽ��� ����ϴ� Ŭ����
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

	// Actor ���� �� ����
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

		ActorInitialize(NewActor, GetSharedFromThis<Ext_Scene>(), NewName.c_str(), 0);
		Actors.insert(std::make_pair(NewName, NewActor));
		
		return std::dynamic_pointer_cast<ActorType>(NewActor);
	}

	// Getter, Setter
	std::shared_ptr<class Ext_Camera> GetMainCamera() { return MainCamera; };
	void SetMainCamera(std::shared_ptr<class Ext_Camera> _MainCamera) 
	{ 
		MainCamera = _MainCamera; 
		Cameras.insert(std::make_pair("MainCamera", MainCamera));
	};

	// ������ �׽�Ʈ��, ���� ����/////////////////////////////////////////////////////
	void RenderTest();

protected:
	virtual void SceneChangeInitialize();
	virtual void SceneChangeEnd();
	virtual void Start() override;
	virtual void Update(float _DeltaTime) override;
	
private:
	void Rendering(); // ������ ������Ʈ
	void ActorInitialize(std::shared_ptr<class Ext_Actor> _Actor, std::weak_ptr<Ext_Scene> _Level, std::string_view _Name, int _Order = 0);

	std::map<std::string, std::shared_ptr<class Ext_Actor>> Actors; // Scene�� ����� Actor��
	std::map<std::string, std::shared_ptr<class Ext_Camera>> Cameras; // Scene�� ����� Camera��
	std::shared_ptr<class Ext_Camera> MainCamera; // ���� Scene�� MainCamera
};