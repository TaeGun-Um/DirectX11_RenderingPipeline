#include "PrecompileHeader.h"
#include "Ext_MeshComponent.h"
#include "Ext_Scene.h"
#include "Ext_Actor.h"
#include "Ext_MeshComponentUnit.h"
#include "Ext_Transform.h"
#include "Ext_Camera.h"

Ext_MeshComponent::Ext_MeshComponent()
{

}

Ext_MeshComponent::~Ext_MeshComponent()
{
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

// 뷰, 프로젝션행렬을 MeshComponent의 Transform에 적용
void Ext_MeshComponent::MeshComponentTransformSetting(std::shared_ptr<Ext_Camera> _Camera)
{
	GetTransform()->SetCameraMatrix(_Camera->GetViewMatrix(), _Camera->GetProjectionMatrix());
}

// 카메라 Rendering에서 여기로 온다음, 각 메시들의 Rendering() 함수를 매틱 실행
void Ext_MeshComponent::Rendering(float _Deltatime, std::shared_ptr<Ext_Camera> _Camera)
{
	MeshComponentTransformSetting(_Camera);
}

