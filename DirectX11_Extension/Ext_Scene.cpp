#include "PrecompileHeader.h"
#include "Ext_Scene.h"

#include "Ext_Actor.h"
#include "Ext_Camera.h"
#include "Ext_MeshComponent.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_DirectXDevice.h"
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
	// Scene 업데이트
}

void Ext_Scene::ActorsUpdate(float _DeltaTime)
{
	this->Update(_DeltaTime); // Scene에서 업데이트 할거 있으면 먼저 하고

	// ──────────────────────────────────────────────────────────────
	//  iterator 무효화 방지 — 순회 중 CreateActor 호출 허용
	// ──────────────────────────────────────────────────────────────
	//  ActorList 를 직접 range-for 로 돌리면, Update 도중 누군가
	//  Scene->CreateActor<T>() 를 호출할 때 내부에서 ActorList 에
	//  push_back 하는 순간 벡터가 재할당되어 현재 참조가 dangling 이 됨.
	//  (증상: 다음 iteration 에서 CurActor->Method() 가 0xDDDD... 로 crash)
	//
	//  해결: 이번 프레임에 돌릴 대상 리스트를 **스냅샷 복사** 해 두고,
	//        새로 생긴 액터는 자연스럽게 다음 프레임부터 Update.
	//  비용: shared_ptr 복사 = atomic refcount 증가, 액터 수준에선 무시 가능.
	//  관련: ReleaseTestActor 가 1초마다 CreateActor 를 호출해 이 패턴을 확실히 건드린다.
	for (auto& [Key, ActorList] : Actors)
	{
		std::vector<std::shared_ptr<Ext_Actor>> Snapshot = ActorList;

		for (const std::shared_ptr<Ext_Actor>& CurActor : Snapshot)
		{
			if (!CurActor || !CurActor->IsUpdate() || CurActor->IsDeath())
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
	// 셰도우 렌더타겟 클리어 실시
	for (auto& Light : Lights)
	{
		std::shared_ptr<Ext_Light> CurLight = Light.second;
		std::shared_ptr<Ext_DirectXRenderTarget> CurShadowTarget = CurLight->GetShadowRenderTarget();
		if (nullptr != CurShadowTarget)
		{
			CurShadowTarget->RenderTargetClear();
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

	// 렌더타겟 Merge
	for (auto& CamIter : Cameras)
	{
		std::shared_ptr<Ext_Camera> CurCamera = CamIter.second;
		std::shared_ptr<Ext_DirectXRenderTarget> CurCamTarget = CurCamera->GetCameraRenderTarget();

		Ext_DirectXDevice::GetMainRenderTarget()->Merge(CurCamTarget);
	}

	Ext_Imgui::Render(GetSharedFromThis<Ext_Scene>(), _DeltaTime);
}

// Actor::Destroy 에서 호출하는 등록 함수 — 대기열에 얹고 플래그만 set
// - 기존 Destroy 는 Actor 자체에 플래그만 세웠고, Scene 은 매 프레임 Actors 전체를 훑어봐야 했음
// - 이제 Scene 이 "누가 죽었는지" 직접 알게 되므로, Release 에서 전체 순회 없이 대기열만 처리
void Ext_Scene::MarkDeadActor(std::shared_ptr<Ext_Actor> _Actor)
{
	if (!_Actor) return;

	PendingDeadActors.push_back(_Actor);
	bHasDeadActor = true;
}

// Scene 단위 죽은 Actor 정리
// 플로우:
//   (1) fast path 가드 — 죽은 게 없으면 즉시 return
//   (2) PendingDeadActors 순회:
//       a. 각 Component 의 OnDetachFromScene() 호출 → 카메라·Scene 레벨 등록 해제
//       b. Actor::Release() 호출 → 내부 Component 자원 정리
//   (3) 원본 Actors 맵에서 IsDeath() 인 항목 제거
//       - 맵은 Order 그룹화라 erase 위치를 빨리 알기 어려워, 어쩔 수 없이 전체 리스트 remove_if
//       - 단 이 경로는 bHasDeadActor true 인 프레임에만 진입
//   (4) 대기열·플래그 초기화
//
// 개선 효과:
//   - 죽은 게 없는 프레임(대부분) → O(1) 으로 종료
//   - 죽은 게 있는 프레임 → OnDetach 는 죽은 것만 처리 (RTTI dynamic_cast 제거)
void Ext_Scene::Release()
{
	if (!bHasDeadActor) return; // fast path

	// [1] 각 죽은 Actor 정리
	for (auto& DeadActor : PendingDeadActors)
	{
		if (!DeadActor) continue;

		// [1-a] 컴포넌트별 씬 레벨 등록 해제 (MeshComp → Camera 제거 등)
		for (const auto& [Name, Comp] : DeadActor->GetComponents())
		{
			if (Comp)
			{
				Comp->OnDetachFromScene();
			}
		}

		// [1-b] Actor 자체 Release — 내부 컴포넌트 자원 정리 + Transform 해제
		DeadActor->Release();
	}

	// [2] 원본 Actors 맵에서 실제 제거
	//     - Order 별로 나뉘어 있어 각 리스트에서 remove_if + erase
	//     - 이 단계는 피할 수 없지만, bHasDeadActor 가드 덕분에 평소엔 진입 자체가 없음
	for (auto& [Order, ActorList] : Actors)
	{
		ActorList.erase(
			std::remove_if(ActorList.begin(), ActorList.end(),
				[](const std::shared_ptr<Ext_Actor>& A) { return !A || A->IsDeath(); }),
			ActorList.end());
	}

	// [3] 대기열·플래그 초기화
	PendingDeadActors.clear();
	bHasDeadActor = false;
}

// Actor 생성 시 자동 호출(이름 설정, 오너 신 설정, 오더 설정)
void Ext_Scene::ActorInitialize(std::shared_ptr<Ext_Actor> _Actor, std::weak_ptr<Ext_Scene> _Level, std::string_view _Name, int _Order /*= 0*/)
{
	_Actor->SetName(_Name);
	_Actor->SetOwnerScene(_Level);
	_Actor->SetOrder(_Order);

	if (nullptr != std::dynamic_pointer_cast<Ext_Light>(_Actor))
	{
		std::shared_ptr<Ext_Light> NewLight = std::dynamic_pointer_cast<Ext_Light>(_Actor);
		PushLight(NewLight, NewLight->GetName());
	}

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