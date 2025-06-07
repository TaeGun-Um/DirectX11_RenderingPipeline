#include "PrecompileHeader.h"
#include "Ext_Scene.h"

#include "Ext_Actor.h"
#include "Ext_Camera.h"
#include "Ext_MeshComponent.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_Light.h"
#include "Ext_Imgui.h"

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
	for (auto& Light : Lights)
	{
		// if (false == Light->IsShadow()) continue;

		std::shared_ptr<Ext_DirectXRenderTarget> ShadowRenderTarget = Light.second->GetShadowRenderTarget();
		// std::shared_ptr<Ext_DirectXRenderTarget> BakeTarget = Light->GetBakeTarget(Light->GetBakeTargetIndex());

		if (nullptr != ShadowRenderTarget)
		{
			ShadowRenderTarget->RenderTargetClear();

			//if (Light->GetLightData().LightType == static_cast<int>(LightType::Point))
			//{
			//	ShadowTarget->MergeCubemap(BakeTarget);
			//}
			//else
			//{
			//	ShadowTarget->Merge(BakeTarget);
			//}
		}
	}

	// Rendering 업데이트
	for (auto& CamIter : Cameras)
	{
		std::shared_ptr<Ext_Camera> CurCamera = CamIter.second;

		CurCamera->CameraTransformUpdate(); // 카메라에 대한 뷰, 프로젝션, 뷰포트 행렬 최신화

		LightDataBuffer.LightCount = 0; // 라이트 업데이트 전, 상수버퍼 갯수 초기화(순회하면서 값 넣어줘야하기 때문)
		for (auto& LightIter : Lights) // 라이트들 돌면서 업데이트
		{
			std::shared_ptr<Ext_Light> CurLight = LightIter.second;
			CurLight->LightUpdate(CurCamera, _DeltaTime);

			LightDataBuffer.Lights[LightDataBuffer.LightCount] = *CurLight->GetLightData().get(); // 상수버퍼에 값 넣어주기
			++LightDataBuffer.LightCount; // 다음 순회를 위해 1 올려주는 작업임
		}

		CurCamera->Rendering(_DeltaTime); // Camera의 MeshComponent들 Rendering
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

// 콜리전들 저장
void Ext_Scene::PushCollision(std::shared_ptr<class Ext_CollisionComponent> _CollisionComponent, int _Order)
{
	Collisions[_Order].push_back(_CollisionComponent);
}

// Light를 Scene에게 저장
void Ext_Scene::PushLight(std::shared_ptr<class Ext_Light> _Light, std::string_view _LightName)
{
	Lights.insert(std::make_pair(_LightName.data(), _Light));
}

// 메인 카메라 세팅
void Ext_Scene::SetMainCamera(std::shared_ptr<Ext_Camera> _MainCamera)
{
	MainCamera = _MainCamera;
	Cameras.insert(std::make_pair("MainCamera", MainCamera));
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

// 디렉셔널 라이트 세팅
void Ext_Scene::SetDirectionalLight(std::shared_ptr<Ext_Light> _Light)
{
	DirectionalLight = _Light;
	DirectionalLight->SetLightType(LightType::Directional);
	DirectionalLight->SetLightRange(10000.0f); // 별로 의미는 없음
}