#pragma once
#include "Ext_Object.h"

// Scene �Ӽ��� ����ϴ� Ŭ����
class Ext_Scene : public Ext_Object
{
	friend class Ext_Core;

public:
	// constrcuter destructer
	Ext_Scene() {}
	~Ext_Scene() {}

	// delete Function
	Ext_Scene(const Ext_Scene& _Other) = delete;
	Ext_Scene(Ext_Scene&& _Other) noexcept = delete;
	Ext_Scene& operator=(const Ext_Scene& _Other) = delete;
	Ext_Scene& operator=(Ext_Scene&& _Other) noexcept = delete;

	// Actor ���� �� ����
	template<typename ActorType>
	std::shared_ptr<ActorType> CreateActor(std::string_view _Name, int _Order = 0)
	{
		std::shared_ptr<Ext_Actor> NewActor = std::make_shared<ActorType>();
		std::string NewName = _Name.data();
		if (_Name == "")
		{
			const type_info& Info = typeid(ActorType);
			NewName = Info.name();
			NewName.replace(0, 6, "");
		}

		ActorInitialize(NewActor, GetSharedFromThis<Ext_Scene>(), NewName.c_str(), _Order);
		Actors[GetOrder()].push_back(NewActor);
		
		return std::dynamic_pointer_cast<ActorType>(NewActor);
	}

	// ī�޶� Getter, Setter
	std::shared_ptr<class Ext_Camera> GetMainCamera() { return MainCamera; };
	void SetMainCamera(std::shared_ptr<class Ext_Camera> _MainCamera) 
	{ 
		MainCamera = _MainCamera; 
		Cameras.insert(std::make_pair("MainCamera", MainCamera));
	};
	std::shared_ptr<Ext_Camera> FindCamera(std::string_view _CameraName);

	// MeshComponent�� MainCamera�� MeshComponents�� push
	void PushMeshToCamera(std::shared_ptr<class Ext_MeshComponent> _MeshComponent, std::string_view _CameraName);

protected:
	virtual void SceneChangeInitialize(); // Scene ���� �� ȣ��
	virtual void SceneChangeEnd(); // Scene ���� �� ȣ��
	virtual void Start() override; // Scene ���� �� ȣ��
	virtual void Update(float _DeltaTime) override; // Actors�� Actor���� Update ȣ��
	
private:
	void Rendering(float _DeltaTime); // ������ ������Ʈ
	void ActorInitialize(std::shared_ptr<class Ext_Actor> _Actor, std::weak_ptr<class Ext_Scene> _Level, std::string_view _Name, int _Order); // Actor ���� �� �ڵ� ȣ��, �̸� ����, Owner����
	
	std::map<int, std::vector<std::shared_ptr<class Ext_Actor>>> Actors;	// Scene�� ����� Actor��, Order�� �׷�ȭ
	std::map<std::string, std::shared_ptr<class Ext_Camera>> Cameras;		// Scene�� ����� Camera��
	std::shared_ptr<class Ext_Camera> MainCamera;									// ���� Scene�� MainCamera
};