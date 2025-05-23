#include "PrecompileHeader.h"
#include "Ext_MeshComponent.h"
#include "Ext_Scene.h"
#include "Ext_Actor.h"
#include "Ext_MeshComponentUnit.h"

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

// 메시 컴포넌트 유닛 만들기
std::shared_ptr<Ext_MeshComponentUnit> Ext_MeshComponent::CreateMeshComponentUnit(std::string_view _MeshName, std::string_view _MaterialName)
{
	std::shared_ptr<Ext_MeshComponentUnit> NewUnit = CreateMeshComponentUnit();
	NewUnit->MeshComponentUnitInitialize(_MeshName, _MaterialName);

	return std::shared_ptr<class Ext_MeshComponentUnit>();
}

// 메시 컴포넌트 유닛 저장
std::shared_ptr<Ext_MeshComponentUnit> Ext_MeshComponent::CreateMeshComponentUnit()
{
	std::shared_ptr<Ext_MeshComponentUnit> NewUnit = std::make_shared<Ext_MeshComponentUnit>();
	NewUnit->SetOwnerMeshComponent(GetSharedFromThis<Ext_MeshComponent>()); // 소유주 설정
	Units.push_back(NewUnit); // 저장

	return NewUnit;
}

void Ext_MeshComponent::Start()
{
	Transform = OwnerActor.lock()->GetTransform();
	PushMeshToCamera("MainCamera");
}

// 카메라 Rendering에서 여기로 온다음, 각 메시들의 Rendering() 함수를 매틱 실행
void Ext_MeshComponent::Rendering(float _Deltatime)
{

}

