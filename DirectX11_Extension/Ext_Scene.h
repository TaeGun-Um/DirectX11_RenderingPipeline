#pragma once
#include "Ext_Object.h"
#include "Ext_Light.h"

// Scene 속성을 담당하는 클래스
class Ext_Scene : public Ext_Object
{
	friend class Ext_Core;
	friend class Ext_Camera;

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
	std::shared_ptr<ActorType> CreateActor(std::string_view _Name, ActorOrder _Order)
	{
		CreateActor(_Name, static_cast<int>(_Order));
	}

	// Actor 생성 및 저장
	template<typename ActorType>
	std::shared_ptr<ActorType> CreateActor(std::string_view _Name, int _Order = 0)
	{
		if (_Name == "")
		{
			MsgAssert("Actor 생성 시에는 이름을 지정해주세요");
			return 0;
		}

		std::shared_ptr<Ext_Actor> NewActor = std::make_shared<ActorType>();
		std::string NewName = _Name.data();

		ActorInitialize(NewActor, GetSharedFromThis<Ext_Scene>(), NewName.c_str(), _Order);
		Actors[GetOrder()].push_back(NewActor);
		
		return std::dynamic_pointer_cast<ActorType>(NewActor);
	}

	// 카메라 Getter, Setter
	std::shared_ptr<class Ext_Camera> GetMainCamera() { return MainCamera; }; // 메인 카메라 가져오기
	void SetMainCamera(std::shared_ptr<class Ext_Camera> _MainCamera); // 메인 카메라 세팅(호출 안하는게 좋음)
	std::shared_ptr<Ext_Camera> FindCamera(std::string_view _CameraName); // 카메라 찾기(메인 카메라밖에 없다)
	std::map<std::string, std::shared_ptr<class Ext_Light>>& GetLights() { return Lights; }

	// Light Getter, Setter
	std::shared_ptr<class Ext_Light> GetDirectionalLight() { return DirectionalLight; }; // 디렉셔널 라이트 가져오기
	const LightDatas& GetLightDataBuffer() { return LightDataBuffer; }; // 디렉셔널 라이트 가져오기
	void SetDirectionalLight(std::shared_ptr<class Ext_Light> _Light); // 디렉셔널 라이트 세팅(씬이 만들어지면 바로 호출 1회)

	// MeshComponent를 MainCamera의 MeshComponents에 push
	void PushMeshToCamera(std::shared_ptr<class Ext_MeshComponent> _MeshComponent, std::string_view _CameraName);

	// Collision을 Scene에게 저장
	void PushCollision(std::shared_ptr<class Ext_CollisionComponent> _CollisionComponent, int _Order);

	// Light를 Scene에게 저장
	void PushLight(std::shared_ptr<class Ext_Light> _Light, std::string_view _LightName);

	std::vector<std::shared_ptr<class Ext_CollisionComponent>>* FindCollsionGroup(int _Order)
	{
		auto Iter = Collisions.find(_Order);
		if (Iter == Collisions.end())
		{
			MsgAssert("없는 충돌체 그룹은 찾을 수 없습니다.");
			return nullptr;
		}

		return &(Iter->second);
	}

protected:
	virtual void SceneChangeInitialize(); // Scene 변경 시 호출
	virtual void SceneChangeEnd(); // Scene 변경 시 호출
	virtual void Start() override; // Scene 생성 시 호출
	virtual void Update(float _DeltaTime) override; // Actors내 Actor들의 Update 호출
	virtual void Release() override;
	
private:
	void Rendering(float _DeltaTime); // 렌더링 업데이트
	void ActorInitialize(std::shared_ptr<class Ext_Actor> _Actor, std::weak_ptr<class Ext_Scene> _Level, std::string_view _Name, int _Order); // Actor 생성 시 자동 호출, 이름 설정, Owner설정
	
	std::map<int, std::vector<std::shared_ptr<class Ext_Actor>>> Actors;  // Scene에 저장된 Actor들, Order로 그룹화
	std::map<int, std::vector<std::shared_ptr<class Ext_CollisionComponent>>> Collisions; // Scene에 저장된 Collision들, Order로 그룹화
	std::map<std::string, std::shared_ptr<class Ext_Camera>> Cameras;    // Scene에 저장된 Camera들
	std::map<std::string, std::shared_ptr<class Ext_Light>> Lights;            // Scene에 저장된 Light들
	std::shared_ptr<class Ext_Camera> MainCamera;                                  // 현재 Scene의 MainCamera
	std::shared_ptr<class Ext_Light> DirectionalLight;                                 // 현재 Scene의 Directional Light
	LightDatas LightDataBuffer; // 현재 Scene Light의 Data들

};