#pragma once
#include "Ext_Object.h"

// Scene 속성을 담당하는 클래스
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

	// Actor 생성 및 저장
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

	// 카메라 Getter, Setter
	std::shared_ptr<class Ext_Camera> GetMainCamera() { return MainCamera; };
	void SetMainCamera(std::shared_ptr<class Ext_Camera> _MainCamera) 
	{ 
		MainCamera = _MainCamera; 
		Cameras.insert(std::make_pair("MainCamera", MainCamera));
	};
	std::shared_ptr<Ext_Camera> FindCamera(std::string_view _CameraName);

	// MeshComponent를 MainCamera의 MeshComponents에 push
	void PushMeshToCamera(std::shared_ptr<class Ext_MeshComponent> _MeshComponent, std::string_view _CameraName);

protected:
	virtual void SceneChangeInitialize(); // Scene 변경 시 호출
	virtual void SceneChangeEnd(); // Scene 변경 시 호출
	virtual void Start() override; // Scene 생성 시 호출
	virtual void Update(float _DeltaTime) override; // Actors내 Actor들의 Update 호출
	
private:
	void Rendering(float _DeltaTime); // 렌더링 업데이트
	void ActorInitialize(std::shared_ptr<class Ext_Actor> _Actor, std::weak_ptr<class Ext_Scene> _Level, std::string_view _Name, int _Order); // Actor 생성 시 자동 호출, 이름 설정, Owner설정
	
	std::map<int, std::vector<std::shared_ptr<class Ext_Actor>>> Actors;	// Scene에 저장된 Actor들, Order로 그룹화
	std::map<std::string, std::shared_ptr<class Ext_Camera>> Cameras;		// Scene에 저장된 Camera들
	std::shared_ptr<class Ext_Camera> MainCamera;									// 현재 Scene의 MainCamera
};