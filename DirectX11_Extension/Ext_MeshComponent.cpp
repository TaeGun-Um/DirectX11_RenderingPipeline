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

// �����ϸ� ī�޶󿡵� �����ϱ� ���� ����
void Ext_MeshComponent::PushMeshToCamera(std::string_view _CameraName)
{
	GetOwnerScene().lock()->PushMeshToCamera(GetSharedFromThis<Ext_MeshComponent>(), _CameraName);
}

// �޽� ������Ʈ�� �ʿ��� ���� ���� �� ����
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

// ��, ������������� MeshComponent�� Transform�� ����
void Ext_MeshComponent::MeshComponentTransformSetting(std::shared_ptr<Ext_Camera> _Camera)
{
	GetTransform()->SetCameraMatrix(_Camera->GetViewMatrix(), _Camera->GetProjectionMatrix());
}

// ī�޶� Rendering���� ����� �´���, �� �޽õ��� Rendering() �Լ��� ��ƽ ����
void Ext_MeshComponent::Rendering(float _Deltatime, std::shared_ptr<Ext_Camera> _Camera)
{
	MeshComponentTransformSetting(_Camera);
}

