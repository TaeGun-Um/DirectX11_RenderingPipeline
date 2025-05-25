#include "PrecompileHeader.h"
#include "Ext_Scene.h"

#include "Ext_Actor.h"
#include "Ext_MeshComponent.h"
#include "Ext_Camera.h"

#include "Ext_DirectXResourceLoader.h" // 임시
#include "Ext_DirectXDevice.h" // 임시

// Actor 생성 시 자동 호출(이름 설정, 오너 신 설정, 오더 설정)
void Ext_Scene::ActorInitialize(std::shared_ptr<Ext_Actor> _Actor, std::weak_ptr<Ext_Scene> _Level, std::string_view _Name, int _Order /*= 0*/)
{
	_Actor->SetName(_Name);
	_Actor->SetOwnerScene(_Level);
	_Actor->SetOrder(_Order);
	_Actor->Start();
}

// 지정한 카메라에 MeshComponent 저장
void Ext_Scene::PushMeshToCamera(std::shared_ptr<Ext_MeshComponent> _MeshComponent, std::string_view _CameraName)
{
	std::shared_ptr<Ext_Camera> FindCam = FindCamera(_CameraName);

	if (nullptr == FindCam)
	{
		MsgAssert("존재하지 않는 카메라에 랜더러를 넣을수는 없습니다.");
		return;
	}

	FindCam->PushMeshComponent(_MeshComponent);
}

// 이름으로 카메라 찾기
std::shared_ptr<Ext_Camera> Ext_Scene::FindCamera(std::string_view _CameraName)
{
	std::map<std::string, std::shared_ptr<Ext_Camera>>::iterator FindIter = Cameras.find(_CameraName.data());

	if (FindIter == Cameras.end())
	{
		return nullptr;
	}

	std::shared_ptr<Ext_Camera> FindCam = FindIter->second;

	return FindCam;
}

void Ext_Scene::SceneChangeInitialize()
{
	// Scene 변경 시 호출
}

void Ext_Scene::SceneChangeEnd()
{
	// Scene 변경 시 호출
}

void Ext_Scene::Start()
{
	// Scene 시작 시 호출
}

// Actors내 Actor들의 Update 호출
void Ext_Scene::Update(float _DeltaTime)
{
	for (auto& [Key, ActorList] : Actors)
	{
		for (const std::shared_ptr<Ext_Actor>& CurActor : ActorList)
		{
			if (!CurActor->GetIsUpdate()) continue;
			else
			{
				CurActor->Update(_DeltaTime);
			}
		}
	}
}

// Camera들의 Rendering 호출
void Ext_Scene::Rendering(float _DeltaTime)
{
	// Rendering 업데이트
	for (auto& Iter : Cameras)
	{
		std::shared_ptr<Ext_Camera> CurCamera = Iter.second;

		CurCamera->CameraTransformUpdate(); // 카메라에 대한 뷰, 프로젝션, 뷰포트 행렬 최신화

		// 임시
		{
			Ext_DirectXDevice::GetContext()->RSSetState(Ext_DirectXResourceLoader::RasterState);
		}

		CurCamera->Rendering(_DeltaTime);
	}
}