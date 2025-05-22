#include "PrecompileHeader.h"
#include "Ext_MeshComponent.h"
#include "Ext_Scene.h"
#include "Ext_Actor.h"

Ext_MeshComponent::Ext_MeshComponent()
{

}

Ext_MeshComponent::~Ext_MeshComponent()
{
}

void Ext_MeshComponent::Start()
{
	Transform = OwnerActor.lock()->GetTransform();
	PushMeshToCamera("MainCamera");
}

void Ext_MeshComponent::PushMeshToCamera(std::string_view _CameraName)
{
	GetOwnerScene().lock()->PushMeshToCamera(GetSharedFromThis<Ext_MeshComponent>(), _CameraName);
}