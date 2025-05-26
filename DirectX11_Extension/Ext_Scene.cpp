#include "PrecompileHeader.h"
#include "Ext_Scene.h"

#include "Ext_Actor.h"
#include "Ext_MeshComponent.h"
#include "Ext_Camera.h"
#include "Ext_Imgui.h"

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
			if (!CurActor->IsUpdate() || CurActor->IsDeath())
			{
				continue;
			}

			CurActor->Update(_DeltaTime); // 내부에서 Component도 같이 호출됨
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
		CurCamera->Rendering(_DeltaTime);
	}

	Ext_Imgui::Render(GetSharedFromThis<Ext_Scene>(), _DeltaTime);
}

// Actors내 Actor들의 Release 호출
void Ext_Scene::Release()
{
	for (auto& [Key, ActorList] : Actors)
	{
		// remove_if은 요소들을 뒤로 밀어버립니다. 이후 erase에서 remove_if가 반환한 새 끝부터 원래 끝까지 실제로 삭제 진행
		ActorList.erase(std::remove_if(ActorList.begin(), ActorList.end(), [](const std::shared_ptr<Ext_Actor>& CurActor)
			{
				if (!CurActor->IsDeath())
				{
					return false; // 죽지 않은 Actor는 유지
				}

				// [1] 카메라에서 MeshComponent 제거 처리
				for (const auto& [Name, Component] : CurActor->GetComponents())
				{
					// MeshComponent인지 확인
					auto MeshComp = std::dynamic_pointer_cast<Ext_MeshComponent>(Component); // 가져온 것 중 MeshComponent인 것은 카메라에서 지워줌
					if (MeshComp)
					{
						// 소유 카메라 얻기
						std::shared_ptr<Ext_Camera> Camera = MeshComp->GetOwnerCamera().lock();
						if (Camera)
						{
							// 카메라에 포함된 메시 컴포넌트, 메시 컴포넌트 유닛 제거
							Camera->RemoveMeshByActor(CurActor);
						}
					}
				}

				// [2] Actor 자체 Destroy 처리
				CurActor->Release();
				return true;
			}
		),
			ActorList.end()
		);
	}
}