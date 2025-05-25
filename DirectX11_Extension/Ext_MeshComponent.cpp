#include "PrecompileHeader.h"
#include "Ext_MeshComponent.h"
#include "Ext_Scene.h"
#include "Ext_Actor.h"
#include "Ext_MeshComponentUnit.h"
#include "Ext_Transform.h"
#include "Ext_Camera.h"

void Ext_MeshComponent::Destroy()
{
	// [1] 유닛 모두 정리
	for (auto& Unit : Units)
	{
		if (Unit)
		{
			Unit->Destroy(); // 아래 정의된 유닛 Destroy 호출
		}
	}
	Units.clear();

	// [2] 카메라 연결 해제
	OwnerCamera.reset();

	// [3] 부모 클래스 Destroy 호출
	Ext_Component::Destroy();
}

// 생성하면 카메라에도 저장하기 위해 실행
void Ext_MeshComponent::PushMeshToCamera(std::string_view _CameraName)
{
	GetOwnerScene().lock()->PushMeshToCamera(GetSharedFromThis<Ext_MeshComponent>(), _CameraName);
}

// 메시 컴포넌트에 필요한 유닛 생성 및 저장
std::shared_ptr<Ext_MeshComponentUnit> Ext_MeshComponent::CreateMeshComponentUnit(std::string_view _MeshName, std::string_view _MaterialName)
{
	std::shared_ptr<Ext_MeshComponentUnit> NewUnit = std::make_shared<Ext_MeshComponentUnit>();
	NewUnit->SetOwnerMeshComponent(GetSharedFromThis<Ext_MeshComponent>());
	NewUnit->MeshComponentUnitInitialize(_MeshName, _MaterialName);
	Units.push_back(NewUnit);

	return NewUnit;
}

void Ext_MeshComponent::Start()
{
	Transform = OwnerActor.lock()->GetTransform();
	PushMeshToCamera("MainCamera");
}

// 각 메시들의 Rendering() 함수를 매틱 실행
void Ext_MeshComponent::Rendering(float _Deltatime, const float4x4& _ViewMatrix, const float4x4& _ProjectionMatrix)
{
	GetTransform()->SetCameraMatrix(_ViewMatrix, _ProjectionMatrix); // 뷰, 프로젝션 세팅
}

